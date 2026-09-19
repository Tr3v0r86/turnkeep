# Hosting Turnkeep

## Current hosting

https://tr3v0r86.github.io/turnkeep/ is a GitHub Pages project site. Its Actions workflow deploys `site/`; relative paths support `/turnkeep/`. Run `python3 tools/release.py` and `python3 -m http.server 8197 --directory site` to preview locally. Immutable prerelease downloads remain on GitHub.

## Keep trevorcardozo.com/turnkeep in the address bar

Use GitHub Pages for the account's root website as well as this project. Create/publish a public repository named exactly `Tr3v0r86.github.io`, containing the chosen root homepage. In THAT repository, Settings → Pages → Custom domain, save `trevorcardozo.com`. Leave the Turnkeep repository's custom-domain field empty: project sites inherit the user site's domain, giving `https://trevorcardozo.com/turnkeep/` automatically. A custom domain field accepts a hostname, not `/turnkeep`.

Recommended before DNS: GitHub account Settings → Pages → Add a domain, then add the exact TXT record GitHub provides in Porkbun and verify ownership. Configure the root site's custom domain before repointing the apex DNS.

In Porkbun: Domain Management → trevorcardozo.com → DNS. Remove only conflicting apex/`www` parking or web-host records and any root URL forwarding. Keep mail MX, verification TXT and unrelated subdomains. Add:

| Type | Host in Porkbun | Answer |
| --- | --- | --- |
| A | blank (root) | 185.199.108.153 |
| A | blank (root) | 185.199.109.153 |
| A | blank (root) | 185.199.110.153 |
| A | blank (root) | 185.199.111.153 |
| CNAME | www | tr3v0r86.github.io |

Default TTL is suitable. Remove stale apex AAAA records pointing at the old host; IPv6 is optional, and GitHub documents the correct addresses if enabled. Keep Porkbun nameservers. Once GitHub's DNS check and certificate complete, enable Enforce HTTPS in the root site's Pages settings. DNS/certificate changes may take up to 24 hours. Verify root, `/turnkeep/`, `/turnkeep/setup.html`, images and downloads. The QR's original GitHub URL must also still reach setup after the domain change; regenerate it for the custom URL only after that URL works.

This setup makes GitHub the host for the domain's root homepage. If the root is later hosted elsewhere, route `/turnkeep/` at that host instead of replacing its DNS blindly. No DNS or root-repository changes were made when writing this guide.

## Forwarding is a different option

Porkbun URL forwarding redirects visitors to another URL; its preserve-path option appends the requested path. It does not mount a project site under your hostname. A 301 from `/turnkeep` to the GitHub URL changes the address bar. Use the root-site arrangement above for the requested persistent custom-domain address.

## Official references

- [GitHub custom domains and inherited project paths](https://docs.github.com/en/pages/configuring-a-custom-domain-for-your-github-pages-site/about-custom-domains-and-github-pages)
- [GitHub DNS records and HTTPS setup](https://docs.github.com/en/pages/configuring-a-custom-domain-for-your-github-pages-site/managing-a-custom-domain-for-your-github-pages-site)
- [Porkbun URL forwarding](https://kb.porkbun.com/article/39-how-to-set-up-url-forwarding)
