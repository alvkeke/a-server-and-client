package com.project.huangchengxi.yxchat;

public class MessageItem {
    //private String text;
    private String SenderId;

    public MessageItem(String SenderId){
        this.SenderId=SenderId;
        //this.text=text;
    }

    public String getSenderId() {
        return SenderId;
    }

    //public String getText() {
        //return text;
    //}
}
