~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fSekrit - f0dder's sekrit notepad - version 1.40 - December 3, 2009
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** Features:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1) tiny self-contained exe note files (50k) which require no installation,
   perfect for running from USB flash drives.
2) strong encryption (256-bit AES in CBC mode)
3) no use of unencrypted temporary files.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** Overview:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fSekrit is a small application for keeping securely encrypted notes. These notes
are truly stand-alone; the editor program and your note are merged together into
a tiny self-contained program file, bypassing the need to install a special
application to view your data. This makes fSekrit ideal for keeping encrypted
notes on, for example, USB flash drives.

Another advantage of using fSekrit is that your un-encrypted data is never
stored on your harddisk.  With a traditional encryption utility you would have
to decrypt your file to disk, view or edit it, and then re-encrypt it. Unless
you use secure file wiping tools, it would be a trivial matter for someone to
retrieve your un-encrypted data, even though you deleted the temporary file.
This is not a viable attack against fSekrit, though, since it never stores your
un-encrypted data on disk.

fSekrit uses very strong encryption to ensure that your data is never at risk.
Rather than using hocus-pocus home-brewed algorithms, fSekrit uses the standard,
military grade, peer-reviewed AES/Rijndael in CBC mode, with a 256-bit keysize.

Self contained fSekrit notefiles are tiny! Only 50k plus the size of your text.
Furthermore, fSekrit should run across the entire windows range 9x/NT/2K/XP,
as well as both 32- and 64-bit versions.



~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** Contact Information:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The preferred method of contact is through the DonationCoder fSekrit subforum,
but feel free to email me if that's what *you* prefer :)

f0dder's email: f0dder@flork.dk
f0dder's homepage: http://f0dder.reteam.org
fsekrit page: http://fsekrit.donationcoder.com
fsekrit forum: http://donationcoder.com/Forums/bb/index.php?board=74.0



~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** License:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This software is provided 'as is' with no explicit or implied warranties in
respect of its properties, including, but not limited to, correctness and/or
fitness for purpose.

fSekrit is copyrighted freeware, and you are not allowed to modify or reverse
engineer it. You're free to redistribute it, as long as it is kept in original,
unaltered form, including this text document. I'd appreciate that you drop me a
note if you re-distribute it - especially if it goes on a magazine cover CD.

Any feedback is of course appreciated as well - be it bug reports, simple
"love it" or "hate it" messages, or feature requests.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** Security aspects / Technical details
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fSekrit started as a request on the superb http://DonationCoder.com [0] forum:
http://www.donationcoder.com/Forums/bb/index.php?topic=1838.0

The AES code is from Dr. Brian Gladman [1], who is the De Facto standard source
when it comes to AES encryption code. Many thanks to Brian for publishing his
source code, and for having a nice license. Speaking about license, the AES code
is "Copyright (c) 2003, Dr Brian Gladman, Worcester, UK.   All rights reserved."

Your unencrypted data is never stored to disk by fSekrit. It is, however, stored
in memory (otherwise you wouldn't be able to view or edit it :-)). This means
that windows, in case of low memory, might choose to swap it out to disk, in
which case a malicious hacker with access to your computer could snoop it from
your paging file. So, avoid running fSekrit in low-memory situations (i.e.
don't keep it open while playing 3D games or using photoshop). Also, if you're
on a public computer, don't keep fSekrit running in the background too long; get
your encrypted text and close down fSekrit.

The 256-bit AES encryption key is constructed directly as a SHA-256 hash of
your passphrase. Note that this hash isn't stored anywhere in your file, it
is only used for the encryption key. Your data is encrypted with 256-bit AES
in CBC mode, with a random IV[5]. This IV is stored in the encrypted note.

Windows will not let an application write to a .exe file that is currently
running, so fSekrit has to jump through some hoops in order to do this.
In previous versions the save process was a bit icky, but with version 1.3
things are a bit cleaner. When you open a fSekrit executable that has encrypted
data, it will copy the editor part (NOT the data part!) of itself to a
temporary file. This temporary file is launched, and used to edit the original
file. This way, your notes can be edited directly.

Once you're done editing and exit fSekrit, the temporary editor executable will
be deleted (this is done by launching your actual note with some special command
line argument).

The fSekrit executable is compressed using Jeremy Collake's PECompact2 [2],
which chopped merrily away at the original ~99k filesize before compression.
This is NOT done to add 'security', but simply to reduce executable filesize.
For tools like fSekrit, security through obscurity don't work, and when I am
satisfied with the state of my program, I will release it under some open source
license for peer review.

To identify whether a correct passphrase has been entered, a SHA-256 hash of
the original plaintext, combined with the IV, is stored in the file header.
This is not a security risk as your data cannot be reconstructed from the hash [3].
Furthermore, by including the IV in the hash, two identical plaintexts will not
yield the same hash value.


Thanks to Olivier Langlois for his CHyperLink class.[4]

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*** References:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

[0] - http://www.donationcoder.com
[1] - http://gladman.plushost.co.uk/oldsite/AES/index.php
[2] - http://www.bitsum.com
[3] - http://en.wikipedia.org/wiki/Cryptographic_hash
[4] - http://www.olivierlanglois.net/hyperlinkdemo.htm
[5] - http://en.wikipedia.org/wiki/Initialization_vector
