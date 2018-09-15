package com.project.huangchengxi.yxchat;

public class ChattingMsgItem {
    static int TYPE_RECEIVE=1;
    static int TYPE_SEND=0;

    private String content;
    private int TYPE;
    public ChattingMsgItem(String content,int TYPE){
        this.content=content;
        this.TYPE=TYPE;
    }

    public int getTYPE() {
        return TYPE;
    }

    public String getContent() {
        return content;
    }
}
