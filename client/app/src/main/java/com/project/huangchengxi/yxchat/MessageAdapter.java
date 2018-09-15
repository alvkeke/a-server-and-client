package com.project.huangchengxi.yxchat;

import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.List;
import java.util.Map;

public class MessageAdapter extends RecyclerView.Adapter<MessageAdapter.ViewHolder>{
    private List<MessageItem> messageItemList;
    private OnClickListener onClickListener;
    private Map<String,List<ChattingMsgItem>> stringListMap;

    public MessageAdapter(List<MessageItem> messageItemList, Map<String,List<ChattingMsgItem>> stringListMap){
        this.messageItemList=messageItemList;
        this.stringListMap=stringListMap;
    }

    class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener{
        TextView msgId;
        TextView msgDetail;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            msgId=(TextView)itemView.findViewById(R.id.msg_item_id);
            msgDetail=(TextView)itemView.findViewById(R.id.msg_text);
            itemView.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            if (onClickListener!=null)
                onClickListener.OnItemClick(v,getAdapterPosition());
        }
    }

    @Override
    public int getItemCount() {
        return messageItemList.size();
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder viewHolder, int i) {
        MessageItem msgItem=messageItemList.get(i);
        viewHolder.msgId.setText(msgItem.getSenderId());
        int show=stringListMap.get(msgItem.getSenderId()).size()-1;
        viewHolder.msgDetail.setText(stringListMap.get(msgItem.getSenderId()).get(show).getContent());
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        View view=LayoutInflater.from(viewGroup.getContext()).inflate(R.layout.layout_message_item,viewGroup,false);
        ViewHolder viewHolder=new ViewHolder(view);
        return viewHolder;
    }

    public interface OnClickListener{
        void OnItemClick(View v,int position);
    }
    public void setOnClickListener(OnClickListener onClickListener){
        this.onClickListener=onClickListener;
    }
}