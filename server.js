import express from 'express'
import { existsSync } from 'fs'
import { spawn } from 'child_process'
import { imgWidth, imgHeight } from './public/common.js'

const app = express()
const port = 3000

app.use(express.static('public'))

app.get('/image', (req, res) => {
  const { x, y, w, i } = req.query
  const imgPath = `/mandelbrot_${x}_${y}_${w}_${i}.png`
  const imgFileName = `public${imgPath}`

  if (existsSync(imgFileName)) {
    console.log('Using exiting cached ', imgFileName)
    res.redirect(imgPath)
    return
  }
  console.log('Generating ', imgFileName)
  const ls = spawn('./mandelbrot', [
    '-o', imgFileName,
    '-x', x,
    '-y', y,
    '-w', w,
    '-i', i,
    '-W', imgWidth,
    '-H', imgHeight
  ])

  ls.stdout.on('data', data => {
    console.log(`stdout: ${data}`)
  })

  ls.stderr.on('data', data => {
    console.log(`stderr: ${data}`)
  })

  ls.on('close', code => {
    console.log(`child process exited with code ${code}`)
    console.log('Generated ', imgFileName)
    res.redirect(imgPath)
  })
})

app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}`)
})
