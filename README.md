Mustache-c
===============

Introduction
----------------
This project implement mustache templates in pure C. It not tend to implement all features, but stick to best ones and take maximum of them.
There is no partials, no html escaping, no set delimiters. However it have very flexible api and could be used in any environment. It pre-compiles
input template, so render would be very fast.

User must supply some callbacks for api to query variable and section values, so you can use, sort of, lambda functions even in simple variable get.
Also it expect from user read and write callbacks - to read template and write to destination, whatever it would be.

Documentation
----------------
 * [Project page](http://x86-64.github.com/mustache-c/)
 * [Documentation](http://x86-64.github.com/mustache-c/html/)

