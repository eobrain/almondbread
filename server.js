import express from 'express'
import { existsSync } from 'fs'
import { spawn } from 'child_process'
import { imgWidth, imgHeight } from './public/common.js'

const app = express()
const port = 3000

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

app.get('/video', async (req, res) => {
  console.log('video', req.query)
  const { x, y, w, i } = req.query
  const videoPath = `/cache/video_${x}_${y}_${w}_${i}.gif`
  const videoFileName = `public${videoPath}`

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

  const videoImgFilenameF = videoW => `public/cache/video_${x}_${y}_${videoW}_${i}.png`

  for (const videoW of videoWs) {
    const videoImgFilename = videoImgFilenameF(videoW)
    console.log('Generating ', videoImgFilename)
    await execute('./mandelbrot', [
      '-o', videoImgFilename,
      '-x', x,
      '-y', y,
      '-w', videoW,
      '-i', i,
      '-W', imgWidth / 5,
      '-H', imgHeight / 5
    ])

    console.log('Generated ', videoImgFilename)
  }
  const inAndOut = [
    ...videoWs.sort((a, b) => a - b),
    ...videoWs.sort((a, b) => b - a)
  ]
  await execute('convert', [
    '-delay', 4,
    '-colors', 200,
    ...inAndOut.map(videoImgFilenameF),
    '-loop', 0,
    videoFileName
  ])
  res.redirect(videoPath)
})

app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}`)
})
