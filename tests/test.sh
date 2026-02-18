#!/bin/bash

buildf="build"
lifecycle="$buildf/lifecycle"
trims="$buildf/trims"
appends="$buildf/appends"
chars="$buildf/chars"
views="$buildf/views"

make

case "$1" in
		lifecycle)
				./$lifecycle ;;
		trims)
				./$trims ;;
		appends)
				./$appends
				;;
		chars)
				./$chars ;;
		views)
				./$views ;;
		*)
				$lifecycle; $trims; $appends; $chars; $views ;;
esac
