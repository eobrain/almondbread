/* global imgElement, xElement, yElement, wElement, iElement */

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

const doit = () => {
  // if (debounce.suppress()) {
  //  return
  // }
  if (window.location.hash) {
    const [x, y, w, i] = window.location.hash.substr(1).split('_')
    xElement.value = x
    yElement.value = y
    wElement.value = w
    iElement.value = i
  }
  const x = xElement.value
  const y = yElement.value
  const w = wElement.value
  const i = iElement.value
  console.log('values:', x, y, w, i)
  imgElement.setAttribute('src', `/image?x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.onclick = event => {
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
  if (!window.location.hash) {
    window.location = '#0_0_8_100'
  }
  doit()
}
