//
//  ViewController.m
//  xmpp
//
//  Created by xuqiqiang on 14-12-24.
//  Copyright (c) 2014年 xuqiqiang. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
}

- (IBAction)textbox:(id)sender {
    [sender resignFirstResponder];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (IBAction)btn_connet:(id)sender {
    
    connect_xmpp("xu123@tt.com", "123", "202.98.157.54", 5223);
}

- (IBAction)btn_set_status:(id)sender {
    const char *str=[_textbox.text UTF8String];
    printf("str:%s\n",str);
    xmpp_set_status(str);
}

- (IBAction)btn_send_chat_message:(id)sender {
    const char *str=[_textbox.text UTF8String];
    printf("%s\n",str);
    xmpp_send_chat_message("xu456@tt.com",str,"aaaaa");
}


- (IBAction)btn_get_friend_list:(id)sender {
    xmpp_get_friends_list();
}


- (IBAction)btn_add_friend:(id)sender {
    const char *str=[_textbox.text UTF8String];
    printf("%s\n",str);
    xmpp_add_friends(str);
}


- (IBAction)btn_delete_friend:(id)sender {
    const char *str=[_textbox.text UTF8String];
    printf("%s\n",str);
    xmpp_delete_friends(str);
}


- (IBAction)btn_send_heartbeat:(id)sender {
    xmpp_send_heartbeat();
}

- (IBAction)btn_declare:(id)sender {
    xmpp_declare_online();
}

- (IBAction)btn_stop:(id)sender {
    stop_xmpp();
}



@end
