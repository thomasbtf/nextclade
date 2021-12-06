#!/usr/bin/env bash

set -o errexit
set -o nounset
set -o pipefail
trap "exit" INT

# Removes "Anonymous function declarations cause Fast Refresh to not preserve local component state. Please add a name to your function." warning.
# Reason: https://github.com/vercel/next.js/issues/15696
sed -i.bak 's/warn=onWarning;/warn=function(){};/g' node_modules/next/dist/build/babel/plugins/no-anonymous-default-export.js

# Removes "<title> should not be used in _document.js" warning.
# Reason: We want title and other SEO tags to be pre-rendered, so that crawlers could find them.
sed -i.bak "s|console.warn(\"Warning: <title> should not be used in _document.js's <Head>. https://err.sh/next.js/no-document-title\");||g" node_modules/next/dist/pages/_document.js


# Removes the line with `"./": "./"` from package.json of the postcss package
# Reason: To avoid this message when using Node.js 16:
#     #(node:3449580) [DEP0148] DeprecationWarning: Use of deprecated folder mapping "./" in the "exports" field module resolution of the package at node_modules/css-loader/node_modules/postcss/package.json.
#     Update this package.json to use a subpath pattern like "./*"
sed -i.bak '/"\.\/": "\.\/"/d' "node_modules/css-loader/node_modules/postcss/package.json"
