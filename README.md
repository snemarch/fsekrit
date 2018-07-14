# fSekrit

fSekrit - "f0dder's sekrit notepad" - is a small, self-contained encrypted notepad application for Windows.

## Getting Started

To get started hacking on fSekrit, check out the code - the project has no external dependencies, so you just need to open the solution in Visual Studio, and upgrade as necessary. VS2017 Community Edition is able to do the trick, and produces working binaries. 

Distribution builds? Don't bother. The setup is written for some 1.x version of SCons, and (for Win9x and NT4 compatibility) requires the VC2003 Toolkit, which isn't publicly available anymore. Target platform support and build system is being heavily reconsidered for "version next".

## Contributing

The project isn't *currently* in a state fit for contributions. There's an amount of work for "version next" that needs to be sorted out and committed first. fSekrit was originally developed using a (private) Subversion repository, and I tried doing too many things at once for the next version... hence "sort things out, commit" rather than converting the Subversion repository to Git.

## Authors

* **Sune Marcher** - [snemarch](https://github.com/snemarch)

## License

This project is licensed under the Mozilla Public License 2.0 - see the [LICENSE.md](LICENSE.md) file for details.

The low-level AES code is "*Copyright (c) 2003, Dr Brian Gladman, Worcester, UK.   All rights reserved.*", with full license terms specified in the files in **src/aes** directory.

The Hyperlink control by Olivier Langlois doesn't come with an explicit license, but came with a "*I hope that the source code will be helpful to you in your projects*" note.

