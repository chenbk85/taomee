use Mail::Sender;



my $mailto = $ARGV[0];  # the recipient list

## 需要动态输入的一些参数，下面的例子是注册成功关于帐号激活的邮件信息,
# perl mail.pl newday.jesse@gmail.com djkdjsk 123456 111111
#可以更改参数，如有需要。。。
my $usr_id = $ARGV[1];
my $active_code = $ARGV[2];
#my $nick = $ARGV[3];

#my $mailto = ('newday.jesse@gmail.com'); # the recipient list
my $subject = "欢迎您加入摩尔庄园";
my @test = "
在摩尔庄园您将化身为一只可爱的红鼻子小鼹鼠摩尔，穿上自己喜欢的摩尔服装，带上自己喜欢的摩尔道具，跟其它的小摩尔一起玩游戏、打水仗、捉迷藏，可以一起来到雪山上，进行一场刺激的摩尔滑雪比赛，美妙的摩尔庄园所有的快乐都由小摩尔们一起创造，一起分享。\r\n
您只需点击下面链接就能激活您的帐号。\n
链接地址：http://10.1.1.5/cgi-bin/mole_emissary.cgi?user_id$usr_id&active_code=$active_code
（如果这个链接点击无效，请拷贝粘贴到您浏览器的URL栏上）
祝您在《摩尔庄园》玩的愉快！!
摩尔庄园公民管理处 www.51mole.com
"
eval {
	(new Mail::Sender {
				on_errors => 'die'
			  }
	)
	->OpenMultipart({smtp=> 'taomee.com', 
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
