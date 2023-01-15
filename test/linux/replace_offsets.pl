#!/usr/bin/perl

use strict;
use warnings;

my $offset_file     = 'offset_database.txt';
my $original_file   = 'cod2_lnxded_1_0.decompiled.original';
my $decompiled_file = 'cod2_lnxded_1_0.decompiled';

print "Opening files...\n";
open(OFFSET_FILE,     '<' . $offset_file)     or die("Error: Cannot open offset file.\n");
open(ORIGINAL_FILE,   '<' . $original_file)   or die("Error: Cannot open original file.\n");
open(DECOMPILED_FILE, '>' . $decompiled_file) or die("Error: Cannot open decompiled file.\n");

print "Parsing offset file...\n";
my %variables;
my $line;
my $key;
while (defined($line = <OFFSET_FILE>)) {
	if ($line =~ /0[xX]0([0-9a-fA-F]+)/) {
		next if ($line =~ /^s?+\/\//);
		my $offset = $1;

		if ($line =~ /\(DWORD\)(\w+\.?\w+)/) {
			my $variable = $1;

			if (defined($variables{$offset})) { die("Error: Duplicate offset found: $offset Aborting.\n"); }

			foreach $key (keys %variables) {
				if ($variable eq $variables{$key}) { die("Error: Duplicate variable found: $variable\n"); }
			}

			$variables{$offset} = $variable;
		}
	}
}

print "Parsing original file...\n";
my $file = '';
while (defined($line = <ORIGINAL_FILE>)) {
	$file = $file . $line;
}

print "Replacing offsets...\n";
foreach $key (keys %variables) {
	$file =~ s/(byte_|dword_|sub_|unk_)$key/$variables{$key}/g;
}

print "Writing decompiled file...\n";
print DECOMPILED_FILE $file;

print "Closing files...\n";
close OFFSET_FILE;
close ORIGINAL_FILE;
close DECOMPILED_FILE;

die "Done!\n";
