#!/bin/bash

buildf="build"
lifecycle="$buildf/lifecycle"
trims="$buildf/trims"
appends="$buildf/appends"
chars="$buildf/chars"

make -B

case "$1" in
		lifecycle)
				./$lifecycle
				;;
		trims)
				./$trims
				;;
		appends)
				./$appends
				;;
		chars)
				./$chars
				;;
		*)
				$lifecycle $trims $appends $chars
				;;
esac
