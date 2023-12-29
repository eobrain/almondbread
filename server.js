import express from 'express'
import { promises, existsSync } from 'fs'
import { spawn } from 'child_process'
import {
  HD_IMG_WIDTH, HD_IMG_HEIGHT,
  MEDIUM_IMG_WIDTH, MEDIUM_IMG_HEIGHT,
  VIDEO_WIDTH, VIDEO_HEIGHT
} from './public/common.js'

const { writeFile } = promises

const app = express()
const port = 3333

if (process.argv.length !== 3) {
  console.error(`Expected 3 arguments but got ${process.argv.map((a, i) => `\n  ${i}: "${a}"`)}`)
  console.error('USAGE: node server.js ./mandelbrot')
  process.exit(1)
}
const executable = process.argv[2]

app.use(express.static('public'))

const execute = (command, args) => new Promise((resolve, reject) => {
  console.log('+', command, args.join(' '))
  const ls = spawn(command, args)

  ls.stdout.on('data', data => {
    console.log(`stdout: ${data}`)
  })

  ls.stderr.on('data', data => {
    console.log(`stderr: ${data}`)
  })

  ls.on('close', code => {
    console.log(`child process exited with code ${code}`)
    if (code) {
      reject(code)
    } else {
      resolve()
    }
  })
})

const imgEndPoint = (imgWidth, imgHeight) => async (req, res) => {
  const { x, y, w, i } = req.query
  const imgPath = `/cache/${imgWidth}x${imgHeight}_${x}_${y}_${w}_${i}.png`
  const imgFileName = `public${imgPath}`

  if (existsSync(imgFileName)) {
    console.log('Using existing cached ', imgFileName)
    res.redirect(imgPath)
    return
  }
  console.log('Generating ', imgFileName)
  await execute(executable, [
    '-o', imgFileName,
    '-x', x,
    '-y', y,
    '-w', w,
    '-i', i,
    '-W', imgWidth,
    '-H', imgHeight
  ])
  console.log('Generated ', imgFileName)
  res.redirect(imgPath)
}

const videoEndPoint = (suffix, imgWidth, imgHeight, fast = true) => async (req, res) => {
  console.log('video', req.query)
  const { x, y, w, i } = req.query
  const videoPath = `/cache/${fast ? '' : 'slow-'}${imgWidth}x${imgHeight}_${x}_${y}_${w}_${i}.${suffix}`
  const videoFileName = `public${videoPath}`
  const inputImagesPath = `public/cache/${imgWidth}x${imgHeight}_${x}_${y}_${w}_${i}.txt`

  if (existsSync(videoFileName)) {
    console.log('Using existing cached ', videoFileName)
    res.redirect(videoPath)
    return
  }

  const videoWs = []
  let mult = fast ? 0.03 : Math.exp(Math.log(2) / 12) - 1
  const start = fast ? w / 10 : w
  for (let videoW = start; videoW < 8; videoW *= (1 + mult)) {
    if (fast) {
      mult *= 1.005
    }
    videoWs.push(videoW)
  }
  if (videoWs.length === 0) {
    res.status(400)
    return
  }
  videoWs.reverse()

  const videoImgFilenameF = frame =>
    `public/cache/${imgWidth}x${imgHeight}_${x}_${y}_${w}_${i}_${frame}.png`

  let inputImages = ''
  let frame = 0
  const frames = []
  for (const videoW of videoWs) {
    ++frame
    frames.push(frame)
    const videoImgFilename = videoImgFilenameF(frame)
    inputImages += `file '${videoImgFilename}'\n`
    if (existsSync(videoImgFilename)) {
      console.log('Using existing cached ', videoImgFilename)
    } else {
      console.log('Generating ', videoImgFilename)
      await execute('./mandelbrot', [
        '-o', videoImgFilename,
        '-x', x,
        '-y', y,
        '-w', videoW,
        '-i', i,
        '-W', imgWidth,
        '-H', imgHeight
      ])

      console.log('Generated ', videoImgFilename)
    }
  }
  await writeFile(inputImagesPath, inputImages)

  const inAndOut = [
    ...frames.sort((a, b) => a - b),
    ...frames.sort((a, b) => b - a)
  ]
  switch (suffix) {
    case 'gif':
      await execute('convert', [
        '-delay', 4,
        '-colors', 200,
        ...inAndOut.map(videoImgFilenameF),
        '-loop', 0,
        videoFileName
      ])
      break
    case 'mp4':
      await execute('ffmpeg', [
        '-r:v', 30,
        '-i', videoImgFilenameF('%d'),
        '-codec:v', 'libx264',
        '-profile:v', 'high',
        '-preset', 'slow',
        '-pix_fmt', 'yuv420p',
        '-crf', 17,
        '-an', videoFileName])
      break
  }
  res.redirect(videoPath)
}

app.get('/gif', videoEndPoint('gif', VIDEO_WIDTH / 5, VIDEO_HEIGHT / 5))
app.get('/zoom', videoEndPoint('mp4', VIDEO_WIDTH / 5, VIDEO_HEIGHT / 5))
app.get('/slow-zoom', videoEndPoint('mp4', VIDEO_WIDTH / 5, VIDEO_HEIGHT / 5, /* fast= */ false))
app.get('/mp4', videoEndPoint('mp4', VIDEO_WIDTH, VIDEO_HEIGHT))
app.get('/hd-mp4', videoEndPoint('mp4', HD_IMG_WIDTH, HD_IMG_HEIGHT, /* fast= */ false))
app.get('/hd', imgEndPoint(HD_IMG_WIDTH, HD_IMG_HEIGHT))
app.get('/medium', imgEndPoint(MEDIUM_IMG_WIDTH, MEDIUM_IMG_HEIGHT))

app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}`)
})
