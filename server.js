import express from 'express'
import { promises, existsSync } from 'fs'
import { spawn } from 'child_process'
import { imgWidth, imgHeight } from './public/common.js'

const { writeFile } = promises

const app = express()
const port = 3333

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

app.get('/image', async (req, res) => {
  const { x, y, w, i } = req.query
  const imgPath = `/cache/mandelbrot_${x}_${y}_${w}_${i}.png`
  const imgFileName = `public${imgPath}`

  if (existsSync(imgFileName)) {
    console.log('Using existing cached ', imgFileName)
    res.redirect(imgPath)
    return
  }
  console.log('Generating ', imgFileName)
  await execute('./mandelbrot', [
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
})

const videoEndPoint = (suffix, scale) => async (req, res) => {
  console.log('video', req.query)
  const { x, y, w, i } = req.query
  const videoPath = `/cache/video_${x}_${y}_${w}_${i}.${suffix}`
  const videoFileName = `public${videoPath}`
  const inputImagesPath = `public/cache/video_${x}_${y}_${w}_${i}.txt`

  if (existsSync(videoFileName)) {
    console.log('Using existing cached ', videoFileName)
    res.redirect(videoPath)
    return
  }

  const videoWs = []
  for (let videoW = w; videoW < 8; videoW *= 1.1) {
    videoWs.push(videoW)
  }
  if (videoWs.length === 0) {
    res.status(400)
    return
  }

  const videoImgFilenameF = frame =>
    `public/cache/${suffix}_${x}_${y}_${w}_${i}_${frame}.png`

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
        '-W', imgWidth / scale,
        '-H', imgHeight / scale
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

app.get('/gif', videoEndPoint('gif', 5))
app.get('/mp4', videoEndPoint('mp4', 1))

app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}`)
})
