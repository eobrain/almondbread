/* global imgElement, xElement, yElement, wElement, iElement */

const debounce = (() => {
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
})()

const doit = () => {
  if (debounce.suppress()) {
    return
  }
  const x = xElement.value
  const y = yElement.value
  const w = wElement.value
  const i = iElement.value
  console.log('values:', x, y, w, i)
  imgElement.setAttribute('src', `/image?x=${x}&y=${y}&w=${w}&i=${i}`)
}

window.onload = () => {
  for (const inputElement of document.querySelectorAll('input')) {
    inputElement.oninput = doit
    doit()
  }
}
