'use strict';
const stage = document.querySelector('[data-product-stage]');
if (stage) {
  const image = stage.querySelector('img');
  const caption = stage.querySelector('[data-view-caption]');
  stage.querySelectorAll('button[data-image]').forEach(button => {
    button.addEventListener('click', () => {
      image.src = button.dataset.image;
      image.alt = button.dataset.alt;
      caption.textContent = button.dataset.caption;
      stage.querySelectorAll('button').forEach(other => other.setAttribute('aria-pressed', String(other === button)));
    });
  });
}
