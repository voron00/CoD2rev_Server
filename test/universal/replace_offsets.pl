#!/usr/bin/perl

use strict;
use warnings;

my $offset_file         = 'offset_database.txt';
my $original_pseudocode = 'cod2_lnxded_1_0.c.original';
my $created_pseudocode  = 'cod2_lnxded_1_0.c';

print "Opening files...\n";
open(OFFSET_FILE,         '<' . $offset_file)         or die("Error: Cannot open offset file.\n");
open(ORIGINAL_PSEUDOCODE, '<' . $original_pseudocode) or die("Error: Cannot open original pseudocode file.\n");
open(CREATED_PSEUDOCODE,  '>' . $created_pseudocode)  or die("Error: Cannot create new pseudocode file.\n");

my %variables;
my $line;
my $key;

print "Parsing offset file...\n";
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

my $file;

print "Parsing original pseudocode file...\n";
$file = '';
while (defined($line = <ORIGINAL_PSEUDOCODE>)) {
	$file = $file . $line;
}

print "Replacing offsets in pseudocode file...\n";
foreach $key (keys %variables) {
	$file =~ s/(byte_|dword_|sub_|unk_)$key/$variables{$key}/g;
}

print "Writing new pseudocode file...\n";
print CREATED_PSEUDOCODE $file;

print "Closing files...\n";
close OFFSET_FILE;
close ORIGINAL_PSEUDOCODE;
close CREATED_PSEUDOCODE;

die "Done!\n";
