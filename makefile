all:
	@if hash meson 2>/dev/null  ; then \
			meson build && \
			ninja -C build && \
			build/rsh ;\
	else \
			echo "rsh uses the meson build system" && \
			echo "please install meson via apt or brew" ; \
	fi

