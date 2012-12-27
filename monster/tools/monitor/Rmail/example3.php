<?php
    /**
    * o------------------------------------------------------------------------------o
    * | This package is licensed under the Phpguru license. A quick summary is       |
    * | that for commercial use, there is a small one-time licensing fee to pay. For |
    * | registered charities and educational institutes there is a reduced license   |
    * | fee available. You can read more  at:                                        |
    * |                                                                              |
    * |                  http://www.phpguru.org/static/license.html                  |
    * o------------------------------------------------------------------------------o
    *
    * © Copyright 2008,2009 Richard Heyes
    */

    /**
    * This example includes setting a Cc: and Bcc: address
    */

    require_once('Rmail.php');

    /**
    * Now create the email it will be attached to
    */
    $mail = new Rmail();
    $mail->setFrom('Richard <richard@example.com>');
    $mail->setSubject('Test email');
    $mail->setCc('fred@example.com');
    $mail->setBcc('barney@example.com');
    $mail->setText('Sample text');
    $result  = $mail->send(array('foo@goo.com'));
?>

Message has been sent