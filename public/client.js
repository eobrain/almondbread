/* global imgElement, xElement, yElement, wElement, iElement, consoleElement */

import { imgWidth, imgHeight } from './common.js'

/* const debounce = (() => {
  let suppressed = false
  const suppress = () => {
    if (suppressed) {
      return true
    }
    suppressed = true
    setTimeout(() => { suppressed = false }, 1000)
    return false
  }
  return { suppress }
})() */

let busy = false

const doit = () => {
  // if (debounce.suppress()) {
  //  return
  // }
  if (!window.location.hash) {
    window.location = '#0_0_8_100'
  }
  const [x, y, w, i] = window.location.hash.substr(1).split('_')
  iElement.value = i
  console.log('values:', x, y, w, i)
  busy = true
  consoleElement.innerText = 'Busy ...'
  imgElement.setAttribute('src', `/image?x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.onload = () => {
    consoleElement.innerText = 'Got image'
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
    xElement.value = newX
    yElement.value = newY
    wElement.value = w / 10
    window.location = `#${newX}_${newY}_${w / 10}_${iElement.value}`
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
