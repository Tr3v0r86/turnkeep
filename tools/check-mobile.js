// Run in the browser console (or gstack browse eval) on the Turnkeep homepage.
(() => {
  const assert = (ok, message) => { if (!ok) throw Error(message); };
  assert(document.documentElement.scrollWidth <= innerWidth, 'Horizontal overflow');
  const folds = [...document.querySelectorAll('.mobile-fold')];
  assert(folds.length === 6, 'Expected mobile disclosures');
  if (innerWidth <= 640) {
    assert(folds.every(fold => !fold.open), 'Phone details should start collapsed');
    const menu = document.querySelector('.mobile-menu');
    menu.querySelector('summary').click();
    assert(menu.open, 'Menu must open');
    menu.querySelector('a[href="#build"]').click();
    assert(!menu.open, 'Menu must close after selection');
    history.replaceState(null, '', '#before');
    dispatchEvent(new Event('hashchange'));
    assert(document.querySelector('#before').closest('details').open, 'Deep links must reveal specifications');
    history.replaceState(null, '', location.pathname);
  } else {
    assert(folds.every(fold => fold.open), 'Desktop details must stay expanded');
  }
  return 'Mobile layout, menu and deep-link checks passed';
})()
