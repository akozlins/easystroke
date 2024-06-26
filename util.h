/*
 * Copyright (c) 2008-2009, Thomas Jaeger <ThJaeger@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef __UTIL_H__
#define __UTIL_H__

#include <glibmm.h>

class Timeout {
	// Invariant: c == &connection || c == nullptr
	sigc::connection *c;
	sigc::connection connection;
	// We have to account for the possibility that timeout() destroys the object
	bool to() { c = nullptr; timeout(); return false; }
public:
	Timeout() : c(0) {}
protected:
	virtual void timeout() = 0;
public:
	bool remove_timeout() {
		if (c) {
			c->disconnect();
			c = 0;
			return true;
		}
		return false;
	}
	void set_timeout(int ms) {
		remove_timeout();
		connection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Timeout::to), ms);
		c = &connection;
	}
	virtual ~Timeout() {
		remove_timeout();
	}
};

#endif
