'use strict';
// Native disclosures remain open without JavaScript; only narrow layouts fold them.
const narrow = matchMedia('(max-width: 640px)');
const folds = [...document.querySelectorAll('.mobile-fold')];
function revealTarget() {
  let target;
  try { target = document.getElementById(decodeURIComponent(location.hash.slice(1))); } catch { return; }
  if (!target) return;
  for (let parent = target; parent; parent = parent.parentElement) {
    if (parent.tagName === 'DETAILS') parent.open = true;
  }
  target.scrollIntoView({behavior: 'instant', block: 'start'});
}
function setLayout() {
  folds.forEach(fold => { fold.open = !narrow.matches; });
  revealTarget();
}
setLayout();
narrow.addEventListener('change', setLayout);
addEventListener('hashchange', revealTarget);
document.querySelector('.mobile-menu nav').addEventListener('click', event => {
  if (narrow.matches && event.target.closest('a')) document.querySelector('.mobile-menu').open = false;
});
