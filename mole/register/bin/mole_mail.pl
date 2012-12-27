use Mail::Sender;



my $mailto = $ARGV[0];  # the recipient list

## 需要动态输入的一些参数，下面的例子是注册成功关于帐号激活的邮件信息,
# perl mail.pl newday.jesse@gmail.com djkdjsk 123456 111111
#可以更改参数，如有需要。。。
my $user_id = $ARGV[1];
my $active_code = $ARGV[2];

#my $mailto = ('newday.jesse@gmail.com'); # the recipient list
my $subject = "Welcome to taomee";
my @text = "尊敬的用户：\r\n欢迎您加入“MOLE庄园”！\r\n请接下来您只需点击下面连接就能激活您的帐号。.
链接地址：
http://192.168.1.5/cgi-bin/mole_emissary.cgi?user_id=$user_id&active_code=$active_code
（如果这个链接点击无效，请拷贝粘贴到你浏览器的URL栏上）
祝您在“MOLE庄园”玩的愉快！!
淘米网络 www.taomee.com";


eval {
	(new Mail::Sender {
				on_errors => 'die'
			  }
	)
	->OpenMultipart({smtp=> 'smtp.taomee.com', 
			 to => $mailto,
			 subject => $subject})

	->Body({ 

		charset => 'UTF-8',
#	       	msg => @text 
		msg => <<'*END*'
*END*

 	     })
	->SendEnc(@text)
#=cut
#	->Attach({
#		       description => 'this file',
#		       ctype => 'application/x-zip-encoded',
#		       encoding => 'Base64',
#		       disposition => 'attachment; filename="mail.pl"; type="txt"',
#		       file => '/home/jesse/mail/mail.pl'
#		       })
#=cut
#	->Close();
 } or print "Error sending mail: $@\n";
