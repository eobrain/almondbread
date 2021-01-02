/* global imgElement, magnificationElement, xElement, yElement, wElement, iElement, consoleElement */

import { imgWidth, imgHeight } from './common.js'

let busy = false

const setCursorMagnification = () => {
  const magnification = magnificationElement.value
  imgElement.className = `cursor-${magnification}`
}

const doit = () => {
  if (!window.location.hash) {
    window.location = '#0_0_8_100'
  }
  const [x, y, w, i] = window.location.hash.substr(1).split('_')
  xElement.innerText = x
  yElement.innerText = y
  wElement.innerText = w
  iElement.value = i
  console.log('values:', x, y, w, i)
  busy = true
  consoleElement.innerText = 'Busy ...'
  imgElement.className = 'cursor-busy'
  imgElement.setAttribute('src', `/image?x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.onload = () => {
    setCursorMagnification()
    consoleElement.innerText = ''
    busy = false
  }
  imgElement.onclick = event => {
    if (busy) {
      imgElement.className = 'cursor-error'
      setTimeout(() => {
        if (busy) {
          imgElement.className = 'cursor-busy'
        }
      }, 200)
      return
    }
    const { offsetX, offsetY } = event
    const scale = w / imgWidth
    const height = scale * imgHeight
    const viewPortLeft = x - w / 2
    const viewPortTop = y - height / 2
    const newX = viewPortLeft + offsetX * scale
    const newY = viewPortTop + offsetY * scale
    console.log({ offsetX, offsetY, newX, newY })
    const magnification = magnificationElement.value
    window.location = `#${newX}_${newY}_${w / (1 << magnification)}_${iElement.value}`
    doit()
  }
}

window.onload = () => {
  window.onhashchange = doit
  magnificationElement.onchange = setCursorMagnification
  doit()
}
