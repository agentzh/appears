#!/usr/bin/env perl

use strict;
use warnings;

my @tokens = qw( G A T C );

my $size_in_GB = shift || 4.8;
die "ERROR: The input size argument should be positive."
    if $size_in_GB <= 0;
my $size = $size_in_GB * (1024 ** 3);
my $i = 0;
while ($i++ < $size) {
    my $ind = int rand 4;
    print $tokens[$ind];
}

