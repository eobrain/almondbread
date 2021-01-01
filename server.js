import express from 'express'
import { spawn } from 'child_process'
import { imgWidth, imgHeight } from './public/common.js'

const app = express()
const port = 3000

app.use(express.static('public'))

app.get('/image', (req, res) => {
  const ls = spawn('./mandelbrot', [
    '-o', 'public/mandelbrot.png',
    '-x', req.query.x,
    '-y', req.query.y,
    '-w', req.query.w,
    '-i', req.query.i,
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
    res.redirect('/mandelbrot.png')
  })
})

app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
})
