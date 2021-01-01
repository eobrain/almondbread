/* global imgElement, magnificationElement, xElement, yElement, wElement, iElement, consoleElement */

import { imgWidth, imgHeight } from './common.js'

let busy = false

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
  imgElement.setAttribute('src', `/image?x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.onload = () => {
    consoleElement.innerText = ''
    busy = false
  }
  imgElement.onclick = event => {
    if (busy) {
      alert('busy')
      return
    }
    const xCoordinate = event.offsetX
    const yCoordinate = event.offsetY
    const scale = w / imgWidth
    const height = scale * imgHeight
    const viewPortLeft = x - w / 2
    const viewPortTop = y - height / 2
    const newX = viewPortLeft + xCoordinate * scale
    const newY = viewPortTop + yCoordinate * scale
    console.log({ xCoordinate, yCoordinate, newX, newY })
    const magnification = magnificationElement.value
    window.location = `#${newX}_${newY}_${w / (1 << magnification)}_${iElement.value}`
    doit()
  }
}

window.onload = () => {
  // for (const inputElement of document.querySelectorAll('input')) {
  //  inputElement.oninput = doit
  //  doit()
  // }
  window.onhashchange = doit
  doit()
}
