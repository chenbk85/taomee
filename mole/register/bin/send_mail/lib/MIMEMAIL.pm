package MIMEMAIL;

#========================================================================================================
#
# This script can send MIME mails with attachments. It uses the MIME::Base64 module for base64-encoding
# and sendmail as MTA.
#
# EXAMPLE:
#
# require 'MIMEMAIL.pm';
# $mail = MIMEMAIL->new('HTML');
#
# $mail->{senderName} = 'sender name';
# $mail->{senderMail} = 'sender@email';
# $mail->{bcc} = 'bcc@email';
#
# $mail->{subject} = "This is the subject line";
#
# $mail->{body} = "Hello! This is a message for you.";   # OR: $mail->{body} = 'path/to/file';
#
# $mail->{attachments}[0] = 'path/to/file1';
# $mail->{attachments}[1] = 'path/to/file2';
# ...
#
# $mail->create();
#
# $recipients = 'recipient1@email,recipient2@email,recipient3@email';
# if(!$mail->send($recipients)) { print $mail->{error}; }
#
#========================================================================================================

use MIME::Base64;

sub initialize {
  my $this = shift;
#--------------------------------------------------------------------------------------------------------
# Configuration
#--------------------------------------------------------------------------------------------------------
  $this->{type} = 'Text';            # default e-mail type ("HTML" or "Text")
  $this->{senderName} = '';          # default sender name
  $this->{senderMail} = '';          # default sender e-mail address
  $this->{cc} = '';                  # default cc (e-mail address)
  $this->{bcc} = '';                 # default bcc (e-mail address)
  $this->{replyTo} = '';             # default reply-to (e-mail address)
  $this->{subject} = '';             # default subject line
  $this->{priority} = 'normal';      # default priority ("high", "normal", "low")

  $this->{documentRoot} = '';        # document root (path to images, stylesheets, etc.)
  $this->{saveDir} = '';             # save e-mail to this directory instead of sending it => just for testing :)
  $this->{charSet} = 'utf-8';   # character set (ISO)
  $this->{useQueue} = 0;             # use mail queue (1 = yes, 0 = no)

  $this->{sendmail} = '/usr/sbin/sendmail';  # path to sendmail

#--------------------------------------------------------------------------------------------------------
# Don't change from here unless you know what you're doing:
#--------------------------------------------------------------------------------------------------------
  %{$this->{inline}} = ();
  @{$this->{attachments}} = ();
  $this->{cnt} = 0;
  $this->{body} = '';
  $this->{header} = '';
  $this->{footer} = '';
  $this->{error} = '';
  $this->{subjectLine} = '';
  $this->{bodyText} = '';
  $this->{uid1} = '';
  $this->{uid2} = '';
  $this->{uid3} = '';
  $this->{created} = 0;
  @{$this->{exclude}} = ('htm', 'php', 'pl', 'prl', 'cgi', 'py', 'asp');
  %{$this->{mimeTypes}} = ('dwg'     => 'application/acad',
                           'asd'     => 'application/astound',
                           'tsp'     => 'application/dsptype',
                           'dxf'     => 'application/dxf',
                           'spl'     => 'application/futuresplash',
                           'gz'      => 'application/gzip',
                           'ptlk'    => 'application/listenup',
                           'hqx'     => 'application/mac-binhex40',
                           'mbd'     => 'application/mbedlet',
                           'mif'     => 'application/mif',
                           'xls'     => 'application/msexcel',
                           'xla'     => 'application/msexcel',
                           'hlp'     => 'application/mshelp',
                           'chm'     => 'application/mshelp',
                           'ppt'     => 'application/mspowerpoint',
                           'ppz'     => 'application/mspowerpoint',
                           'pps'     => 'application/mspowerpoint',
                           'pot'     => 'application/mspowerpoint',
                           'doc'     => 'application/msword',
                           'dot'     => 'application/msword',
                           'bin'     => 'application/octet-stream',
                           'oda'     => 'application/oda',
                           'pdf'     => 'application/pdf',
                           'ai'      => 'application/postscript',
                           'eps'     => 'application/postscript',
                           'ps'      => 'application/postscript',
                           'rtc'     => 'application/rtc',
                           'smp'     => 'application/studiom',
                           'tbk'     => 'application/toolbook',
                           'vmd'     => 'application/vocaltec-media-desc',
                           'vmf'     => 'application/vocaltec-media-file',
                           'xhtml'   => 'application/xhtml+xml',
                           'bcpio'   => 'application/x-bcpio',
                           'z'       => 'application/x-compress',
                           'cpio'    => 'application/x-cpio',
                           'csh'     => 'application/x-csh',
                           'dcr'     => 'application/x-director',
                           'dir'     => 'application/x-director',
                           'dxr'     => 'application/x-director',
                           'dvi'     => 'application/x-dvi',
                           'evy'     => 'application/x-envoy',
                           'gtar'    => 'application/x-gtar',
                           'hdf'     => 'application/x-hdf',
                           'php'     => 'application/x-httpd-php',
                           'phtml'   => 'application/x-httpd-php',
                           'latex'   => 'application/x-latex',
                           'mif'     => 'application/x-mif',
                           'nc'      => 'application/x-netcdf',
                           'cdf'     => 'application/x-netcdf',
                           'nsc'     => 'application/x-nschat',
                           'sh'      => 'application/x-sh',
                           'shar'    => 'application/x-shar',
                           'swf'     => 'application/x-shockwave-flash',
                           'cab'     => 'application/x-shockwave-flash',
                           'spr'     => 'application/x-sprite',
                           'sprite'  => 'application/x-sprite',
                           'sit'     => 'application/x-stuffit',
                           'sca'     => 'application/x-supercard',
                           'sv4cpio' => 'application/x-sv4cpio',
                           'sv4crc'  => 'application/x-sv4crc',
                           'tar'     => 'application/x-tar',
                           'tcl'     => 'application/x-tcl',
                           'tex'     => 'application/x-tex',
                           'texinfo' => 'application/x-texinfo',
                           'texi'    => 'application/x-texinfo',
                           't'       => 'application/x-troff',
                           'tr'      => 'application/x-troff',
                           'roff'    => 'application/x-troff',
                           'troff'   => 'application/x-troff',
                           'ustar'   => 'application/x-ustar',
                           'src'     => 'application/x-wais-source',
                           'zip'     => 'application/zip',
                           'au'      => 'audio/basic',
                           'snd'     => 'audio/basic',
                           'es'      => 'audio/echospeech',
                           'tsi'     => 'audio/tsplayer',
                           'vox'     => 'audio/voxware',
                           'aif'     => 'audio/x-aiff',
                           'aiff'    => 'audio/x-aiff',
                           'aifc'    => 'audio/x-aiff',
                           'dus'     => 'audio/x-dspeeh',
                           'cht'     => 'audio/x-dspeeh',
                           'mid'     => 'audio/x-midi',
                           'midi'    => 'audio/x-midi',
                           'mp2'     => 'audio/x-mpeg',
                           'ram'     => 'audio/x-pn-realaudio',
                           'ra'      => 'audio/x-pn-realaudio',
                           'rpm'     => 'audio/x-pn-realaudio-plugin',
                           'stream'  => 'audio/x-qt-stream',
                           'wav'     => 'audio/x-wav',
                           'dwf'     => 'drawing/x-dwf',
                           'cod'     => 'image/cis-cod',
                           'ras'     => 'image/cmu-raster',
                           'fif'     => 'image/fif',
                           'gif'     => 'image/gif',
                           'ief'     => 'image/ief',
                           'jpeg'    => 'image/jpeg',
                           'jpg'     => 'image/jpeg',
                           'jpe'     => 'image/jpeg',
                           'tiff'    => 'image/tiff',
                           'tif'     => 'image/tiff',
                           'mcf'     => 'image/vasa',
                           'wbmp'    => 'image/vnd.wap.wbmp',
                           'fh4'     => 'image/x-freehand',
                           'fh5'     => 'image/x-freehand',
                           'fhc'     => 'image/x-freehand',
                           'pnm'     => 'image/x-portable-anymap',
                           'pbm'     => 'image/x-portable-bitmap',
                           'pgm'     => 'image/x-portable-graymap',
                           'ppm'     => 'image/x-portable-pixmap',
                           'rgb'     => 'image/x-rgb',
                           'xwd'     => 'image/x-windowdump',
                           'xbm'     => 'image/x-xbitmap',
                           'xpm'     => 'image/x-xpixmap',
                           'csv'     => 'text/comma-separated-values',
                           'css'     => 'text/css',
                           'htm'     => 'text/html',
                           'html'    => 'text/html',
                           'shtml'   => 'text/html',
                           'js'      => 'text/javascript',
                           'txt'     => 'text/plain',
                           'rtx'     => 'text/richtext',
                           'rtf'     => 'text/rtf',
                           'tsv'     => 'text/tab-separated-values',
                           'wml'     => 'text/vnd.wap.wml',
                           'wmlc'    => 'application/vnd.wap.wmlc',
                           'wmls'    => 'text/vnd.wap.wmlscript',
                           'wmlsc'   => 'application/vnd.wap.wmlscriptc',
                           'xml'     => 'text/xml',
                           'etx'     => 'text/x-setext',
                           'sgm'     => 'text/x-sgml',
                           'sgml'    => 'text/x-sgml',
                           'talk'    => 'text/x-speech',
                           'spc'     => 'text/x-speech',
                           'mpeg'    => 'video/mpeg',
                           'mpg'     => 'video/mpeg',
                           'mpe'     => 'video/mpeg',
                           'qt'      => 'video/quicktime',
                           'mov'     => 'video/quicktime',
                           'viv'     => 'video/vnd.vivo',
                           'vivo'    => 'video/vnd.vivo',
                           'avi'     => 'video/x-msvideo',
                           'movie'   => 'video/x-sgi-movie',
                           'vts'     => 'workbook/formulaone',
                           'vtts'    => 'workbook/formulaone',
                           '3dmf'    => 'x-world/x-3dmf',
                           '3dm'     => 'x-world/x-3dmf',
                           'qd3d'    => 'x-world/x-3dmf',
                           'qd3'     => 'x-world/x-3dmf',
                           'wrl'     => 'x-world/x-vrml');
}

sub new {
  my $type = $_[1];
  my $self = {};

  bless $self;
  $self->initialize();
  if($type) { $self->{type} = $type; }

  return $self;
}

sub get_img_type {
  my ($this, $data) = @_;
  my $abc = substr($data, 0, 20);
  my $ftype = '';

  if($abc =~ /GIF/i) { $ftype = 'gif'; }
  elsif($abc =~ /JFIF/i || $abc =~ /Exif/i) { $ftype = 'jpeg'; }
  elsif($abc =~ /PNG/i) { $ftype = 'png'; }
  elsif($abc =~ /FWS/i || $abc =~ /CWS/i) { $ftype = 'swf'; }

  return $ftype;
}

sub get_inl_data {
  my ($this, $css, $html, @m) = @_;
  my ($i, $j, $ftype, @data, $fname, $ext, $incl, $doc_root, $inlName, $repl);
  my $host = $ENV{HTTP_HOST};
  $host =~ s/\/$//;
  $host = "http://$host";

  for($i = 0; $i < int(@{$m[0]}); $i++) {
    $ftype = $ext = $fname = '';
    @data = ();

    if($m[2][$i] !~ m/^(http|ftp|mailto|javascript)/i) {
      $inlName = $m[2][$i];
      $ext = substr($inlName, rindex($inlName, '.') + 1);
      $incl = 1;

      for($j = 0; $j < int(@{$this->{exclude}}) && $incl; $j++) {
        if($ext =~ $this->{exclude}[$j]) { $incl = 0; }
      }

      if($incl) {
        if($this->{documentRoot}) {
          $doc_root = $this->{documentRoot};

          while($inlName =~ m/^\.\.\//) {
            $inlName = substr($inlName, 3);
            $doc_root =~ s/\/[^\/]+$//;
          }
          $fname = "$doc_root/$inlName";
        }
        else { $fname = $inlName; }

        if(open(DAT, '<' . $fname)) {
          binmode(DAT);
          @data = <DAT>;
          close(DAT);
        }
      }
    }

    if(int(@data)) {
      if(!$ext) { $ftype = $this->get_img_type($data[0]); }
      else { $ftype = $ext; }

      if($css) { $repl = $m[1][$i] . '(cid:' . $inlName . ')'; }
      else { $repl = $m[1][$i] . '="cid:' . $inlName . '"'; }
      $html =~ s|$m[0][$i]| $repl|g;

      if(!$this->{inline}{$ftype}{$inlName}) {
        $this->{inline}{$ftype}{$inlName} = MIME::Base64::encode(join('', @data));
      }
    }
    elsif($m[2][$i] !~ m/^(http|ftp|mailto|javascript)/i) {
      if($css) { $repl = $m[1][$i] . "($host/$inlName)"; }
      else { $repl = $m[1][$i] . "=\"$host/$inlName\""; }
      $html =~ s|$m[0][$i]| $repl|g;
    }
  }
  return $html;
}

sub check_body {
  my $this = shift;
  my @m = ();

  @m = _match_all($this->{body}, ' (src|background|href)="?([^" >]+)"?');
  if(int(@{$m[0]})) { $this->{body} = $this->get_inl_data(0, $this->{body}, @m); }
  @m = _match_all($this->{body}, ' (url)\(([^\)]+)\)');
  if(int(@{$m[0]})) { $this->{body} = $this->get_inl_data(1, $this->{body}, @m); }

  $this->{body} =~ s/<(table|tr|div)([^>]*)>\r?\n?/<$1$2>\n/gi;
  $this->{body} =~ s/<\/(table|tr|td|style|script|div|p)>\r?\n?/<\/$1>\n/gi;
}

sub make_boundaries {
  my $this = shift;

  $this->{uid1} = 'Next_' . $ENV{SERVER_NAME} . time . 1;
  $this->{uid2} = 'Next_' . $ENV{SERVER_NAME} . time . 2;
  $this->{uid3} = 'Next_' . $ENV{SERVER_NAME} . time . 3;
}

sub build_header {
  my $this = shift;
  my ($priority, $ms_priority, $alternative);

  $this->{header} = "Return-Path: $this->{senderMail}\n" .
                    "From: $this->{senderName} <$this->{senderMail}>\n" .
                    "X-Sender: $this->{senderMail}\n" .
                    "X-Mailer: MIMEmail (Perl)\n" .
                    "MIME-Version: 1.0\n" .
                    "Subject: $this->{subject}\n";

	if($this->{replyTo}) { $this->{header} .= "Reply-To: " . $this->{replyTo} . "\n"; }
  if($this->{cc}) { $this->{header} .= "Cc: $this->{cc}\n"; }
  if($this->{bcc}) { $this->{header} .= "Bcc: $this->{bcc}\n"; }

  $this->{priority} = lc($this->{priority});
  if($this->{priority} eq 'high') { $priority = 1; $ms_priority = 'high'; }
  elsif($this->{priority} eq 'low') { $priority = 5; $ms_priority = 'low'; }
  else { $priority = 3; $ms_priority = 'normal'; }

  $this->{header} .= "X-Priority: $priority\n" .
                     "X-MSMail-Priority: $ms_priority\n";

  if(int(@{$this->{attachments}})) {
    $this->{header} .= "Content-Type: multipart/mixed; boundary=\"$this->{uid1}\"\n\n" .
                       "This is a multi-part message in MIME format.\n\n" .
                       "--$this->{uid1}\n";
  }

  if($this->{type} eq 'HTML') {
    $alternative = $this->{body};
    $alternative =~ s/<[a-z\/!][^>]*>//gi;
    $alternative =~ s/(\s*\r?\n\s*){2}/$1$1/g;
    $this->{header} .= "Content-Type: multipart/alternative; boundary=\"$this->{uid3}\"\n\n" .
                       "--$this->{uid3}\n";
    $this->{header} .= "Content-Type: text/plain; " .
                       "charset=\"$this->{charSet}\"\n" .
                       "Content-Transfer-Encoding: 8bit\n\n" .
                       "$alternative\n\n" .
                       "--$this->{uid3}\n";

    if(keys(%{$this->{inline}})) {
      $this->{header} .= "Content-Type: multipart/related; boundary=\"$this->{uid2}\"\n\n" .
                         "--$this->{uid2}\n";
    }
  }

  $this->{header} .= "Content-Type: text/" . (($this->{type} eq 'HTML') ? 'html' : 'plain') . "; " .
                     "charset=\"$this->{charSet}\"\n" .
                     "Content-Transfer-Encoding: 8bit\n\n";
}

sub build_footer {
  my $this = shift;
  my (%atts, %ftypes, @file);
  my ($att, $filename, $ext, $ftype, $data, $inlName, $inlType);
  $this->{footer} = '';

  foreach $att (@{$this->{attachments}}) {
    if($att && $att ne 'none') {
      if(open(DAT, '<' . $att)) {
        binmode(DAT);
        $att =~ s/\\/\//g;
        $filename = substr($att, rindex($att, '/') + 1);
        @file = <DAT>;
        close(DAT);

        $ext = substr($filename, rindex($filename, '.') + 1);
        $ftypes{$filename} = $this->{mimeTypes}{$ext} ? $this->{mimeTypes}{$ext} : $this->{mimeTypes}{'bin'};
        $atts{$filename} = MIME::Base64::encode(join('', @file));
      }
    }
  }

  if(keys(%{$this->{inline}})) {
    while($ftype = each(%{$this->{inline}})) {
      while(($inlName, $data) = each(%{$this->{inline}{$ftype}})) {
        $inlType = $this->{mimeTypes}{$ftype} ? $this->{mimeTypes}{$ftype} : $this->{mimeTypes}{'bin'};
        $this->{footer} .= "--$this->{uid2}\n" .
                           "Content-Type: $inlType; name=\"$inlName\"\n" .
                           "Content-ID: <$inlName>\n" .
                           "Content-Disposition: inline; filename=\"$inlName\"\n" .
                           "Content-Transfer-Encoding: base64\n\n" .
                           "$data\n\n";
      }
    }
    $this->{footer} .= "--$this->{uid2}--\n\n";
  }

  if($this->{type} eq 'HTML') { $this->{footer} .= "--$this->{uid3}--" . (keys(%atts) ? "\n\n" : ''); }

  if(keys(%atts)) {
    while(($filename, $file) = each(%atts)) {
      $this->{footer} .= "--$this->{uid1}\n" .
                         "Content-Type: $ftypes{$filename}; name=\"$filename\"\n" .
                         "Content-Disposition: attachment; filename=\"$filename\"\n" .
                         "Content-Transfer-Encoding: base64\n\n" .
                         "$file\n\n";
    }
    $this->{footer} .= "--$this->{uid1}--";
  }
}

sub send {
  my ($this, @recipients) = @_;
  my $ok = 0;

  if($this->{created}) {
    $this->build_header();
    my $recipients = join(', ', @recipients);
    my $mimemail = "To: $recipients\n" . $this->{header} . $this->{body} . "\n\n" . $this->{footer};
    my ($options, $file);

    if($this->{saveDir}) {
      $file = $this->{saveDir} . '/mail_' . ($this->{cnt} + 1) . '.eml';

      if($ok = open(DAT, '>' . $file)) {
        print DAT $mimemail;
        close(DAT);
      }
      else { $this->{error} = 'Could not open "' . $file . '"'; }
    }
    else {
      $options = ' -t -i' . ($this->{useQueue} ? ' -odq' : '') . ' -f ' . $this->{senderMail};
      if($ok = open(SENDMAIL, '|' . $this->{sendmail} . $options)) {
        print SENDMAIL $mimemail;
        close(SENDMAIL);
      }
      else {
        $this->{error} = 'Error while sending e-mail';
        if($recipients !~ m/,/) { $this->{error} .= ' to "' . $recipients . '"'; }
      }
    }
    $this->{cnt}++;
    $this->{subject} = $this->{subjectLine};
    $this->{body} = $this->{bodyText};
  }
  else { $this->{error} = 'MIME mail not created yet'; }

  return $ok;
}

sub create {
  my $this = shift;
  my (@content, $filename);
  %{$this->{inline}} = ();

  if(length($this->{body}) < 100) {
    $filename = $this->{body};
    $filename =~ s/\\/\//g;

    if(open(DAT, '<' . $filename)) {
      @content = <DAT>;
      close(DAT);
      $this->{body} = join('', @content);
      $this->{documentRoot} = substr($filename, 0, rindex($filename, '/'));
    }
  }

  if($this->{type} eq 'HTML') { $this->check_body(); }
  $this->make_boundaries();
  $this->build_footer();
  $this->{subjectLine} = $this->{subject};
  $this->{bodyText} = $this->{body};
  $this->{created} = 1;
}

#--------------------------------------------------------------------------------------------------------
# Global Functions
#--------------------------------------------------------------------------------------------------------

sub _match_all {
  my ($str, $reg) = @_;
  my @m = ();
  my ($cnt, $val);
  my $i = 0;

  while($str =~ m/$reg/gi) {
    $m[0][$i] = $&;
    $cnt = 1;
    while($val = eval('$' . $cnt)) { $m[$cnt++][$i] = $val; }
    $i++;
  }
  return @m;
}

#--------------------------------------------------------------------------------------------------------

return 1;
