/* global
 imgElement,
 zoomElement,
 magnificationElement,
 gifElement,
 mp4SmallElement,
 slowMp4SmallElement,
 mp4Element,
 hdMp4Element,
 mediumElement,
 xElement,
 yElement,
 wElement,
 iElement,
 iNewElement,
 iLog10Element */

import { HD_IMG_WIDTH, HD_IMG_HEIGHT } from './common.js'

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
    window.location = '#0_0_8_1000'
  }
  const [x, y, w, i] = window.location.hash.substr(1).split('_')
  xElement.innerText = x
  yElement.innerText = y
  wElement.innerText = w
  iElement.innerText = i
  console.log('values:', x, y, w, i)
  busy = true
  gifElement.setAttribute('href', `/gif?x=${x}&y=${y}&w=${w}&i=${i}`)
  mp4SmallElement.setAttribute('href', `/zoom?x=${x}&y=${y}&w=${w}&i=${i}`)
  slowMp4SmallElement.setAttribute('href', `/slow-zoom?x=${x}&y=${y}&w=${w}&i=${i}`)
  mp4Element.setAttribute('href', `/mp4?x=${x}&y=${y}&w=${w}&i=${i}`)
  hdMp4Element.setAttribute('href', `/hd-mp4?x=${x}&y=${y}&w=${w}&i=${i}`)
  mediumElement.setAttribute('href', `/medium?x=${x}&y=${y}&w=${w}&i=${i}`)
  imgElement.className = 'cursor-busy'
  imgElement.setAttribute('src', `/hd?x=${x}&y=${y}&w=${w}&i=${i}`)
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
    const scale = w / HD_IMG_WIDTH
    const height = scale * HD_IMG_HEIGHT
    const viewPortLeft = x - w / 2
    const viewPortTop = y - height / 2
    const newX = viewPortLeft + offsetX * scale
    const newY = viewPortTop + (HD_IMG_HEIGHT - offsetY) * scale
    const magnification = magnificationElement.valueAsNumber
    const zoom = 1 << magnification
    zoomElement.innerText = zoom
    window.location = `#${newX}_${newY}_${w / zoom}_${iNewElement.innerText}`
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
