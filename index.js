const express = require('express')
const { spawn } = require('child_process')

const app = express()
const port = 3000

app.use(express.static('public'))

app.get('/image', (req, res) => {
  const ls = spawn('./mandelbrot', ['-o', 'public/mandelbrot.png', '-i', '1000'])

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
