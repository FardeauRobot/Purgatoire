#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>

std::ostream& endofline(std::ostream& os);

// Regular colors
# define BLACK          "\033[30m"
# define RED            "\033[31m"
# define GREEN          "\033[32m"
# define YELLOW         "\033[33m"
# define BLUE           "\033[34m"
# define MAGENTA        "\033[35m"
# define CYAN           "\033[36m"
# define WHITE          "\033[37m"

// Bold colors
# define BOLD_BLACK     "\033[1;30m"
# define BOLD_RED       "\033[1;31m"
# define BOLD_GREEN     "\033[1;32m"
# define BOLD_YELLOW    "\033[1;33m"
# define BOLD_BLUE      "\033[1;34m"
# define BOLD_MAGENTA   "\033[1;35m"
# define BOLD_CYAN      "\033[1;36m"
# define BOLD_WHITE     "\033[1;37m"

// Underline colors
# define UL_BLACK       "\033[4;30m"
# define UL_RED         "\033[4;31m"
# define UL_GREEN       "\033[4;32m"
# define UL_YELLOW      "\033[4;33m"
# define UL_BLUE        "\033[4;34m"
# define UL_MAGENTA     "\033[4;35m"
# define UL_CYAN        "\033[4;36m"
# define UL_WHITE       "\033[4;37m"

// Background colors
# define BG_BLACK       "\033[40m"
# define BG_RED         "\033[41m"
# define BG_GREEN       "\033[42m"
# define BG_YELLOW      "\033[43m"
# define BG_BLUE        "\033[44m"
# define BG_MAGENTA     "\033[45m"
# define BG_CYAN        "\033[46m"
# define BG_WHITE       "\033[47m"

// Text styles
# define BOLD            "\033[1m"
# define DIM             "\033[2m"
# define ITALIC          "\033[3m"
# define UNDERLINE       "\033[4m"
# define BLINK           "\033[5m"
# define REVERSE         "\033[7m"
# define STRIKETHROUGH   "\033[9m"

// Reset
# define RESET           "\033[0m"

// ====== MACROS ======

#define PORT 8080
#define BACK_LOG 128
#define TIMEOUT 1000

#define STD_BUFFER 4096

// ====== ENUMS =======
// STATUS CODES
enum e_codes {

	// SUCCESS
	OK = 200, // The default success. `GET` worked, body contains the resource.
	CREATED = 201, // Resource was created. Should include a `Location:` header pointing to the new resource.
	NO_CONTENT = 204, // Success but no body. Headers still terminate with `\r\n\r\n`, but no bytes after.

	// REDIRECTION
	/*
		Originally (HTTP/1.0), 301 and 302 were defined as preserving the method. In practice, **browsers broke that rule** for 302 (and sometimes 301): if you POST and get a 302, browsers will follow with a GET. This was nonsensical but became de-facto behaviour.

		HTTP/1.1 ratified the practical reality by adding:
		- **303** — "follow with GET, always" (codifies the broken-302 behaviour)
		- **307** — "follow with the same method, always" (codifies the original 302 spec)
		- **308** — "follow with the same method, always, and remember it" (permanent 307)

		**Modern recommendation:**
		- Want permanent redirect, method-preserving? → **`308`**
		- Want temporary redirect, method-preserving? → **`307`**
		- Want to force a GET after a POST? → **`303`**
		- Want a permanent redirect for a GET-only resource (typical for site moves)? → **`301`** still works

		For webserv's config-driven redirects, `301` and `302` are almost always sufficient. The config might look like:

		```nginx-ish
		location /old-path {
			return 301 /new-path;
		}	
	*/
	MOVED_PERMANENTLY = 301, // This URL is gone forever; use the new one from now on	
	FOUND = 302, // This URL is temporarily elsewhere; come back here for the original
	SEE_OTHER = 303, // Use the new URL with `GET`, regardless of original method
	TEMPORARY_REDIRECT = 307, // Like 302 but explicitly preserves the method
	PERMANENT_REDIRECT = 308, //Like 301 but explicitly preserves the method  

	// CLIENT ERROR
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWD = 405,
	REQUEST_TIMEOUT = 408,
	LENGTH_REQUIRED = 411,
	PAYLOAD_TOO_LARGE = 413,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	REQUEST_HEADER_TOO_LARGE = 431,

	// SERVER ERROR
	INTERNAL_SERV_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	VERSION_NOT_SUPPORTED = 505
} ;

// METHODS
enum e_methods {
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	PATCH,
	OPTIONS,
	CONNECT,
	TRACE
};



#endif
