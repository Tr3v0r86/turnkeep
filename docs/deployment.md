# Hosting Turnkeep

## GitHub Pages

The public site is designed for https://tr3v0r86.github.io/turnkeep/. Relative asset and download URLs support the repository subpath. GitHub Actions packages the allowlisted sources and deploys `site/` with Pages. Select **GitHub Actions** under Settings → Pages.

The website is plain HTML/CSS. No database, analytics, cookies or external font requests are needed. To edit it, change `site/index.html` and assets, run `python3 tools/release.py`, and preview locally. Review new files before adding them to `RELEASE_FILES.txt`.

## trevorcardozo.com/turnkeep

Configure a permanent **301 redirect** from both `/turnkeep` and `/turnkeep/` to `https://tr3v0r86.github.io/turnkeep/` on the service handling trevorcardozo.com. Preserve the remainder of the existing domain. DNS alone cannot redirect a URL path. The domain was parking at the time of release preparation, so the path rule must be installed when an HTTP host or redirect service is configured.

For an existing Netlify/Cloudflare Pages `_redirects` file, add:

```text
/turnkeep https://tr3v0r86.github.io/turnkeep/ 301
/turnkeep/ https://tr3v0r86.github.io/turnkeep/ 301
```

For an existing nginx server block:

```nginx
location = /turnkeep { return 301 https://tr3v0r86.github.io/turnkeep/; }
location = /turnkeep/ { return 301 https://tr3v0r86.github.io/turnkeep/; }
```

These are alternatives for the selected host, not instructions to replace the apex site. Verify the final HTTP status and destination after installing the rule.
