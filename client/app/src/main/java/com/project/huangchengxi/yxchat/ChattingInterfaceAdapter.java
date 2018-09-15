package com.project.huangchengxi.yxchat;

import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.List;

public class ChattingInterfaceAdapter extends RecyclerView.Adapter<ChattingInterfaceAdapter.ViewHolder>{
    private List<ChattingMsgItem> chattingMsgItemList;
    private OnImageClickListener onImageClickListener;

    public ChattingInterfaceAdapter(List<ChattingMsgItem> chattingMsgItemList){
        this.chattingMsgItemList=chattingMsgItemList;
    }

    class ViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener{
        ImageView imageView_user_info;
        TextView leftText;
        TextView rightText;
        LinearLayout leftLayout;
        LinearLayout rightLayout;
        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            leftText=(TextView)itemView.findViewById(R.id.left_msg_text);
            rightText=(TextView)itemView.findViewById(R.id.right_msg_text);
            leftLayout=(LinearLayout)itemView.findViewById(R.id.left_layout);
            rightLayout=(LinearLayout)itemView.findViewById(R.id.right_layout);
            imageView_user_info=(ImageView)itemView.findViewById(R.id.image_view_user_info);
            imageView_user_info.setOnClickListener(this);
        }

        @Override
        public void onClick(View v) {
            if (onImageClickListener!=null)
                onImageClickListener.OnItemClick(v,getAdapterPosition());
        }
    }

    @Override
    public int getItemCount() {
        return chattingMsgItemList.size();
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder viewHolder, int i) {
        ChattingMsgItem chattingMsgItem=chattingMsgItemList.get(i);
        if (chattingMsgItem.getTYPE()==ChattingMsgItem.TYPE_RECEIVE){
            viewHolder.leftLayout.setVisibility(View.VISIBLE);
            viewHolder.rightLayout.setVisibility(View.GONE);
            viewHolder.leftText.setText(chattingMsgItem.getContent());
        }else{
            viewHolder.leftLayout.setVisibility(View.GONE);
            viewHolder.rightLayout.setVisibility(View.VISIBLE);
            viewHolder.rightText.setText(chattingMsgItem.getContent());
        }
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup viewGroup, int i) {
        View view= LayoutInflater.from(viewGroup.getContext()).inflate(R.layout.layout_chatting_msg_item,viewGroup,false);
        ViewHolder viewHolder=new ViewHolder(view);
        return viewHolder;
    }
    public interface OnImageClickListener {
        void OnItemClick(View v,int position);
    }

    public void setOnImageClickListener(OnImageClickListener onImageClickListener) {
        this.onImageClickListener = onImageClickListener;
    }
}
