/* global
 imgElement,
 zoomElement,
 magnificationElement,
 videoElement,
 negPowerOf2Element,
 xElement,
 yElement,
 wElement,
 iElement,
 iNewElement,
 iLog10Element */

import { imgWidth, imgHeight } from './common.js'

let busy = false

const setCursorMagnification = () => {
  const magnification = magnificationElement.valueAsNumber
  zoomElement.innerText = 1 << magnification
  imgElement.className = `cursor-${magnification}`
}

const setIterations = () => {
  const i = Math.pow(10, iLog10Element.valueAsNumber)
  iNewElement.innerText = i
}

const doit = () => {
  if (!window.location.hash) {
    window.location = '#20_0_0_8388608_1000'
  }
  const [p, x, y, w, i] = window.location.hash.substr(1).split('_')
  negPowerOf2Element.innerText = p
  xElement.innerText = x
  yElement.innerText = y
  wElement.innerText = w
  iElement.innerText = i
  console.log('values:', x, y, w, i)
  busy = true
  videoElement.setAttribute('href', `/video?p=${p}&x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.className = 'cursor-busy'
  imgElement.setAttribute('src', `/image?p=${p}&x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.onload = () => {
    setCursorMagnification()
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
    const newY = viewPortTop + (imgHeight - offsetY) * scale
    const magnification = magnificationElement.valueAsNumber
    const zoom = 1 << magnification
    zoomElement.innerText = zoom
    window.location = `#${p}_${newX}_${newY}_${w}_${iNewElement.innerText}`
    doit()
  }
}

window.onload = () => {
  window.onhashchange = doit
  magnificationElement.onchange = setCursorMagnification
  iLog10Element.onchange = setIterations
  setIterations()
  doit()
}
