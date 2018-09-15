package com.project.huangchengxi.yxchat;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Rect;
import android.net.sip.SipSession;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private final String COMMAND_BACK_SUCCESS   =   "COMMAND=BACK=SUCCESS";
    private final String COMMAND_BACK_FAILED    =   "COMMAND=BACK=FAILED=";
    private final String COMMAND_LIST_BEGIN     =   "COMMAND=LIST=BEGIN==";
    private final String COMMAND_LIST_END       =   "COMMAND=LIST=END====";
    private final String COMMAND_LIST_DATA	    =	"COMMAND=LIST=DATA===";
    private final String COMMAND_LIST_EMPTY	    =	"COMMAND=LIST=EMPTY==";
    private final String COMMAND_KICK_NOTFOUND  =   "COMMAND=KCIK=NOFOUND";
    private final String COMMAND_SEND_NOTFOUND  =   "COMMAND=SEND=NOFOUND";
    private final String COMMAND_SERVER_BUSY    =   "COMMAND=SERVER=BUSY=";
    private final String COMMAND_REPEAT_ID	    =   "COMMAND=REPEAT=ID===";
    private final String COMMAND_LOGIN          =   "COMMAND=REGISTER=ME=";
    private final String COMMAND_LOGOUT         =   "COMMAND=DELETE=ME===";
    private final String COMMAND_CONFIRM_TYPE   =   "COMMAND=COMFIRM=TYPE";
    private final String COMMAND_READY          =   "COMMAND=BEGIN=ACTION";
    private final String COMMAND_RETURN_TYPE    =   "COMMAND=RETURN=TYPE=";
    private final String COMMAND_HELPSEND_TO    =   "COMMAND=HELPSEND=TO=";
    private final String COMMAND_GET_CLIENTS    =   "COMMAND=GET=ONLINES=";
    private final String COMMAND_TRANSLATE      =   "COMMAND=TRANSLATE===";
    private final int NATTYPE_NOATNAT	=0	;	//不处于NAT之后
    private final int NATTYPE_FULLCONE =1	;	//任何请求都能通过的该ip和端口传送
    private final int NATTYPE_RESTCONE= 2	;	//只有端口向某个主机发送过信息,才能接收该主机的消息
    private final int NATTYPE_PORTCONE =3	;	//只有端口向某个主机的某个端口发送过信息,才能接收该主机从该端口发送过来的信息
    private final int NATTYPE_SYMMETRIC= 4 ;//只有和相同外部IP和相同外部端口的主机通信才能够映射出相同的NAT转换
    private final int HD_LOGIN_ERROR		=	1000;
    private final int HD_LOGIN_SUCCESS	=	1001;
    private final int PORT_CMD 	=9880;
    private final int PORT_SER 	=9881;
    private final int PORT_NAT 	=9882;
    private final int PORT_LST	=9883;
    private final int MAXDATA 	=1024;
    private final int MAXCMD 	=	128;
    private int NAT_TYPE;
    private ViewPager viewPager;
    private ArrayList<View> viewArrayList;
    private RecyclerView recyclerViewMsg;
    private RecyclerView recyclerViewUser;
    private RecyclerView recyclerViewMsgDetail;
    private EditText userNickname;
    private EditText IPAddress;
    private Button LoginButton;
    private Button SendButton;
    private EditText messageTextContent;
    private List<ChattingMsgItem> chattingMsgItemList=new ArrayList<>();
    private List<MessageItem> messageItemList=new ArrayList<>();
    private List<UserItem> userItemList=new ArrayList<>();
    private HashMap<String,List<ChattingMsgItem>> MsgListHashMap=new HashMap<>();
    private String[] mTitle=new String[]{"消息","在线用户","关于"};
    private View MsgView;
    private View UserView;
    private View aboutView;
    private LinearLayout LoginLayout;
    private LinearLayout MainLayout;
    private LinearLayout ChattingLayout;
    private ActionBar actionBar;
    //网络通讯成员
    private static DatagramSocket datagramSocket;
    private static DatagramPacket datagramPacket;
    private static DatagramPacket datagramPacketIn;
    private int PORT=9881;
    private String IP;
    private String UserNickName;
    //用于操作主线程的handler
    private Handler MyHandler;
    //列表适配器
    private MessageAdapter messageAdapter;
    private UserAdapter userAdapter;
    private ChattingInterfaceAdapter chattingInterfaceAdapter;
    //当前通信界面的用户id
    private String CurrentUserId;
    private String dataOutString;
    private String dataInString;
    private Thread MessageListener;
    //监听返回键
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch(keyCode){
            case KeyEvent.KEYCODE_BACK:
                //按下返回键
                if (ChattingLayout.getVisibility()== View.VISIBLE)
                    showMain();
                else {
                    AlertDialog.Builder alert=new AlertDialog.Builder(this);
                    alert.setTitle("确定退出");
                    alert.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });

                    alert.setNegativeButton("取消", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            //do nothing
                        }
                    });

                    alert.show();
                }
                return true;
            default:
                return super.onKeyDown(keyCode, event);
        }
    }

    @SuppressLint({"InflateParams", "HandlerLeak"})
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        MsgView=LayoutInflater.from(this).inflate(R.layout.layout_message,null);
        UserView=LayoutInflater.from(this).inflate(R.layout.layout_ol_user,null);
        aboutView=LayoutInflater.from(this).inflate(R.layout.layout_about,null);
        actionBar=getSupportActionBar();
        if (actionBar != null) {
            actionBar.hide();
        }
        //昵称和IP输入框
        userNickname= findViewById(R.id.user_nickname);
        IPAddress= findViewById(R.id.ip_address);
        LoginButton= findViewById(R.id.login_button);
        SendButton= findViewById(R.id.send_button);
        messageTextContent= findViewById(R.id.input_edit_message_text);
        LoginLayout= findViewById(R.id.login_layout);
        MainLayout= findViewById(R.id.main_form_layout);
        ChattingLayout= findViewById(R.id.chatting_layout);
        viewPager= findViewById(R.id.view_pager);
        //将布局存进数组中
        viewArrayList= new ArrayList<>();
        viewArrayList.add(MsgView);
        viewArrayList.add(UserView);
        viewArrayList.add(aboutView);
        recyclerViewMsg= MsgView.findViewById(R.id.recycler_view_message);
        recyclerViewUser= UserView.findViewById(R.id.recycler_view_ol_user);
        recyclerViewMsgDetail= findViewById(R.id.chatting_recycler_view);
        recyclerViewUser.setLayoutManager(new LinearLayoutManager(this));
        recyclerViewMsg.setLayoutManager(new LinearLayoutManager(this));
        recyclerViewMsgDetail.setLayoutManager(new LinearLayoutManager(this));
        messageAdapter=new MessageAdapter(messageItemList,MsgListHashMap);
        userAdapter=new UserAdapter(userItemList);
        chattingInterfaceAdapter=new ChattingInterfaceAdapter(chattingMsgItemList);
        recyclerViewMsg.setAdapter(messageAdapter);
        recyclerViewUser.setAdapter(userAdapter);
        recyclerViewMsgDetail.setAdapter(chattingInterfaceAdapter);
        //隐藏主布局 先显示登陆布局
        showLogin();
        messageAdapter.setOnClickListener(new MessageAdapter.OnClickListener() {
            @Override
            public void OnItemClick(View v, int position) {
                //初始化对话窗口界面控件
                MessageItem messageItem=messageItemList.get(position);
                CurrentUserId=messageItem.getSenderId();
                chattingInterfaceAdapter=new ChattingInterfaceAdapter(MsgListHashMap.get(CurrentUserId));
                chattingInterfaceAdapter.setOnImageClickListener(new ChattingInterfaceAdapter.OnImageClickListener() {
                    @Override
                    public void OnItemClick(View v,int position) {
                        String UserIP=null;
                        int nat=0;
                        int port=0;
                        Intent intent=new Intent(MainActivity.this,UserDetail.class);
                        for (UserItem userItem:userItemList){
                            if (userItem.getUserId().equals(CurrentUserId)){
                                UserIP=userItem.getIP();
                                nat=userItem.getNat();
                                port=userItem.getPort();
                            }
                        }
                        intent.putExtra("UserId",CurrentUserId);
                        intent.putExtra("UserIP",UserIP);
                        intent.putExtra("NAT",nat);
                        intent.putExtra("PORT",port);
                        startActivity(intent);
                    }
                });
                recyclerViewMsgDetail.setAdapter(chattingInterfaceAdapter);
                showChatting();
                chattingInterfaceAdapter.notifyDataSetChanged();
            }
        });
        userAdapter.setOnClickListener(new UserAdapter.OnClickListener() {
            @Override
            public void OnItemClick(View view, int position) {
                UserItem userItem=userItemList.get(position);
                CurrentUserId=userItem.getUserId();
                if (!MsgListHashMap.containsKey(CurrentUserId)){
                    List<ChattingMsgItem> list=new ArrayList<>();
                    MsgListHashMap.put(CurrentUserId,list);
                    chattingInterfaceAdapter=new ChattingInterfaceAdapter(MsgListHashMap.get(CurrentUserId));
                }else{
                    chattingInterfaceAdapter=new ChattingInterfaceAdapter(MsgListHashMap.get(CurrentUserId));
                }
                chattingInterfaceAdapter.setOnImageClickListener(new ChattingInterfaceAdapter.OnImageClickListener() {
                    @Override
                    public void OnItemClick(View v,int positon) {
                        String UserIP=null;
                        int nat=0;
                        int port=0;
                        Intent intent=new Intent(MainActivity.this,UserDetail.class);
                        for (UserItem userItem:userItemList){
                            if (userItem.getUserId().equals(CurrentUserId)){
                                UserIP=userItem.getIP();
                                nat=userItem.getNat();
                                port=userItem.getPort();
                            }
                        }
                        intent.putExtra("UserId",CurrentUserId);
                        intent.putExtra("UserIP",UserIP);
                        intent.putExtra("NAT",nat);
                        intent.putExtra("PORT",port);
                        startActivity(intent);
                    }
                });
                recyclerViewMsgDetail.setAdapter(chattingInterfaceAdapter);
                chattingInterfaceAdapter.notifyDataSetChanged();
                showChatting();
            }
        });

        //ViewPager 的适配器
        viewPager.setAdapter(new PagerAdapter() {
            @Override
            public int getCount() {
                return viewArrayList.size();
            }
            @Override
            public boolean isViewFromObject(@NonNull View view, @NonNull Object o) {
                return view==o;
            }
            @Override
            public void destroyItem(@NonNull ViewGroup container, int position, @NonNull Object object) {
                container.removeView(viewArrayList.get(position));
            }
            @NonNull
            @Override
            public Object instantiateItem(@NonNull ViewGroup container, int position) {
                container.addView(viewArrayList.get(position));
                return viewArrayList.get(position);
            }

            @Nullable
            @Override
            public CharSequence getPageTitle(int position) {
                return mTitle[position];
            }

        });
        //点击消息监听器
        LoginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (userNickname.getText().toString().equals("") || IPAddress.getText().toString().equals("")){
                    Toast.makeText(MainActivity.this,"用户名或IP地址不能为空",Toast.LENGTH_SHORT).show();
                }else{
                    Thread LoginThread=new Thread(new LoginButtonClick());
                    LoginButton.setClickable(false);
                    LoginThread.start();
                }
            }
        });
        //发送消息
        SendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dataOutString=messageTextContent.getText().toString();
                if (dataOutString.equals(""))Toast.makeText(MainActivity.this,"内容不能为空",Toast.LENGTH_SHORT).show();
                else{
                    ChattingMsgItem chattingMsgItem=new ChattingMsgItem(dataOutString,ChattingMsgItem.TYPE_SEND);
                    dataOutString=COMMAND_HELPSEND_TO+CurrentUserId+"|"+UserNickName+"|"+dataOutString;
                    (MsgListHashMap.get(CurrentUserId)).add(chattingMsgItem);
                    chattingInterfaceAdapter.notifyDataSetChanged();
                    boolean doesExit=false;
                    for (MessageItem messageItem1:messageItemList){
                        if (messageItem1.getSenderId().equals(CurrentUserId)){
                            doesExit=true;
                            break;
                        }
                    }
                    if (!doesExit){
                        messageItemList.add(new MessageItem(CurrentUserId));
                    }
                    messageAdapter.notifyDataSetChanged();
                    chattingInterfaceAdapter.notifyDataSetChanged();
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                datagramPacket=new DatagramPacket(dataOutString.getBytes(),dataOutString.getBytes().length,InetAddress.getByName(IP),PORT_SER);
                                datagramSocket.send(datagramPacket);
                            }catch (Exception e){
                                e.printStackTrace();
                            }
                        }
                    }).start();
                    messageTextContent.setText("");
                }
            }
        });
        MyHandler=new Handler(){
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                switch (msg.what){
                    case 1:
                        //登陆成功 转回主界面 并接收服务器发送过来的数据
                        MessageListener=new Thread(new MessageListener());
                        MessageListener.start();
                        showMain();
                        actionBar.show();
                        //请求获取在线
                        break;
                    case 3:
                        messageAdapter.notifyDataSetChanged();
                        userAdapter.notifyDataSetChanged();
                        chattingInterfaceAdapter.notifyDataSetChanged();
                        messageAdapter.notifyItemInserted(messageItemList.size()-1);
                        recyclerViewMsg.scrollToPosition(messageItemList.size()-1);
                        chattingInterfaceAdapter.notifyItemInserted(chattingMsgItemList.size()-1);
                        recyclerViewMsgDetail.scrollToPosition(chattingMsgItemList.size()-1);
                        break;
                    case 5:
                        Toast.makeText(MainActivity.this, dataInString, Toast.LENGTH_SHORT).show();
                        datagramSocket.close();
                        LoginButton.setClickable(true);
                        break;
                    case 6:
                        Toast.makeText(MainActivity.this,"用户名重复",Toast.LENGTH_SHORT).show();
                        datagramSocket.close();
                        LoginButton.setClickable(true);
                        break;
                    case 8:
                        Toast.makeText(MainActivity.this,"连接服务器超时",Toast.LENGTH_SHORT).show();
                        Toast.makeText(MainActivity.this, dataInString, Toast.LENGTH_SHORT).show();
                        datagramSocket.close();
                        LoginButton.setClickable(true);
                        //连接服务器超时
                        break;
                    default:
                        break;
                }
            }
        };
    }
    private class LoginButtonClick implements Runnable{
        private byte[] dataIn=new byte[MAXDATA];
        @Override
        public void run() {
            try{
                //绑定本地端口 设置连接超时时间为10s
                datagramSocket=new DatagramSocket(PORT);
                datagramSocket.setSoTimeout(10000);

                datagramPacketIn=new DatagramPacket(dataIn,dataIn.length);
                UserNickName=userNickname.getText().toString();
                IP=IPAddress.getText().toString();
                //给服务器发送客户端登陆的消息
                datagramPacket=new DatagramPacket((COMMAND_LOGIN+UserNickName).getBytes(),(COMMAND_LOGIN+UserNickName).getBytes().length,InetAddress.getByName(IP),PORT_SER);
                datagramSocket.send(datagramPacket);
                //等待服务器发送判断nat类型的指令
                datagramSocket.receive(datagramPacketIn);
                if ((dataInString=new String(dataIn,0,datagramPacketIn.getLength())).equals(COMMAND_CONFIRM_TYPE)){
                    //给服务器返回确认开始的信息
                    datagramPacket=new DatagramPacket((COMMAND_READY+UserNickName).getBytes(),(COMMAND_READY+UserNickName).getBytes().length,InetAddress.getByName(IP),PORT_NAT);
                    datagramSocket.send(datagramPacket);
                    //开始判断nat类型
                    datagramSocket.receive(datagramPacketIn);
                    //等待判断完成  --服务器发送命令，返回nat类型   --存储nat类型
                    dataInString=new String(dataIn,0,datagramPacketIn.getLength());
                    NAT_TYPE=Integer.parseInt(dataInString.substring(20));
                    dataInString=dataInString.substring(0,20);
                    if (dataInString.equals(COMMAND_BACK_SUCCESS)){
                        //至此登陆完成
                        //自动运行一次获取在线用户列表
                        //datagramPacket = new DatagramPacket((COMMAND_GET_CLIENTS + UserNickName).getBytes(), (COMMAND_GET_CLIENTS + UserNickName).getBytes().length, InetAddress.getByName(IP), PORT_SER);
                        //datagramSocket.send(datagramPacket);
                        Message message=MyHandler.obtainMessage();
                        message.what=1;
                        MyHandler.sendMessage(message);
                    }else{
                        Message message=MyHandler.obtainMessage();
                        message.what=5;
                        MyHandler.sendMessage(message);
                    }
                }
                else if (dataInString.equals(COMMAND_REPEAT_ID)) {
                    Message message=MyHandler.obtainMessage();
                    message.what=6;
                    MyHandler.sendMessage(message);
                }else {
                    Message message=MyHandler.obtainMessage();
                    message.what=5;
                    MyHandler.sendMessage(message);
                }
            }catch(Exception e){
                datagramSocket.close();
                Message message=MyHandler.obtainMessage();
                message.what=8;
                MyHandler.sendMessage(message);
                e.printStackTrace();
            }
        }
    }

    //全局监听器  用于检测服务器发过来的所有消息  然后进行判断信息类型
    private String msgStringIn;
    class MessageListener implements Runnable{
        byte[] msgIn=new byte[MAXDATA];
        byte[] dataIn=new byte[MAXDATA];
        String COMMAND;
        Message message;
        MessageItem messageItem;
        ChattingMsgItem chattingMsgItem;
        String[] Info;
        @Override
        public void run() {
            try{
                datagramPacket = new DatagramPacket((COMMAND_GET_CLIENTS + UserNickName).getBytes(), (COMMAND_GET_CLIENTS + UserNickName).getBytes().length, InetAddress.getByName(IP), PORT_SER);
                datagramSocket.send(datagramPacket);
            }catch (Exception e){
                e.printStackTrace();
            }
            while (true){
                try{
                datagramPacketIn=new DatagramPacket(msgIn,msgIn.length);
                datagramSocket.receive(datagramPacketIn);
                msgStringIn=new String(msgIn,0,datagramPacketIn.getLength());
                COMMAND=msgStringIn.substring(0,20);
                //筛选信息类型
                switch (COMMAND){
                    case COMMAND_TRANSLATE:  //接收到服务器的中转消息
                        /*
                         *命令+id| + 信息
                         */
                        dataInString=msgStringIn.substring(20,msgStringIn.length());
                        Info=dataInString.split("\\|",-1);
                        messageItem=new MessageItem(Info[0]);
                        boolean doesExit=false;
                        for (MessageItem messageItem:messageItemList){
                            if (messageItem.getSenderId().equals(Info[0])) {
                                doesExit = true;
                                break;
                            }
                        }
                        if (doesExit){
                            //如果已经在消息列表中了  就直接更新内容
                            chattingMsgItem=new ChattingMsgItem(Info[1],ChattingMsgItem.TYPE_RECEIVE);
                            MsgListHashMap.get(Info[0]).add(chattingMsgItem);
                        }else{
                            chattingMsgItem=new ChattingMsgItem(Info[1],ChattingMsgItem.TYPE_RECEIVE);
                            chattingMsgItemList=new ArrayList<>();
                            chattingMsgItemList.add(chattingMsgItem);
                            MsgListHashMap.put(Info[0],chattingMsgItemList);
                            messageItemList.add(messageItem);
                        }
                        message=MyHandler.obtainMessage();
                        message.what=3;
                        MyHandler.sendMessage(message);
                        break;
                    case COMMAND_SEND_NOTFOUND:
                        message=MyHandler.obtainMessage();
                        message.what=3;
                        MyHandler.sendMessage(message);
                        break;
                    case COMMAND_LIST_BEGIN:      //开始接收用户列表
                        /*
                         * COMMAND_LIST_DATA + IP + : + PORT + : + ID + : + nat type
                        */
                        System.out.println(COMMAND);
                        UserItem userItem;
                        //循环接收用户信息直到接收到COMMAND_LIST_END
                        datagramPacketIn=new DatagramPacket(dataIn,dataIn.length);
                        userItemList.clear();
                        int itocount = 0;
                        while (true){
                            Thread.sleep(100);
                            datagramPacket=new DatagramPacket((COMMAND_READY+UserNickName).getBytes(),(COMMAND_READY+UserNickName).getBytes().length,InetAddress.getByName(IP),PORT_LST);
                            datagramSocket.send(datagramPacket);
                            Thread.sleep(100);
                            datagramSocket.receive(datagramPacketIn);
                            dataInString=new String(dataIn,0,datagramPacketIn.getLength());
                            System.out.println(dataInString);
                            if (dataInString.equals(COMMAND_LIST_END))
                                break;
                            if(itocount++ == 2){
                                System.out.println("Error in receive clients list.");
                                break;
                            }
                            dataInString=dataInString.substring(20,dataInString.length());
                            Info=dataInString.split(":");
                            userItem=new UserItem(Info[2],Info[0],Integer.parseInt(Info[3]),Integer.parseInt(Info[1]));
                            userItemList.add(userItem);

                        }
                        message=MyHandler.obtainMessage();
                        message.what=3;
                        MyHandler.sendMessage(message);
                        break;
                        /*
                    case COMMAND_LIST_DATA:  //单独处理用户上线
                        //System.out.println(COMMAND);
                        Info=dataInString.split(":");
                        userItem=new UserItem(Info[2],Info[0],Integer.parseInt(Info[3]),Integer.parseInt(Info[1]));
                        userItemList.add(userItem);
                        message=MyHandler.obtainMessage();
                        message.what=3;
                        MyHandler.sendMessage(message);
                        break;*/
                    //目前还不了解用途
                    case COMMAND_BACK_SUCCESS:
                        break;
                    case COMMAND_BACK_FAILED:
                        break;
                    default:
                        break;
                }
            } catch (Exception e){
                    e.printStackTrace();
                }
            }
        }
    }
    private void showChatting(){
        LoginLayout.setVisibility(View.GONE);
        MainLayout.setVisibility(View.GONE);
        ChattingLayout.setVisibility(View.VISIBLE);
    }
    private void showLogin(){
        LoginLayout.setVisibility(View.VISIBLE);
        MainLayout.setVisibility(View.GONE);
        ChattingLayout.setVisibility(View.GONE);
    }
    private void showMain(){
        LoginLayout.setVisibility(View.GONE);
        MainLayout.setVisibility(View.VISIBLE);
        ChattingLayout.setVisibility(View.GONE);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater menuInflater=getMenuInflater();
        menuInflater.inflate(R.menu.bar_layout,menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.menu:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                datagramPacket=new DatagramPacket((COMMAND_GET_CLIENTS+UserNickName).getBytes(),(COMMAND_GET_CLIENTS+UserNickName).getBytes().length,InetAddress.getByName(IP),PORT_SER);
                                datagramSocket.send(datagramPacket);
                        }catch (Exception e){
                            e.printStackTrace();
                        }
                        }
                    }).start();
                break;
            case R.id.logout:
                AlertDialog.Builder al=new AlertDialog.Builder(this);
                al.setCancelable(true);
                al.setTitle("确定注销账号");
                al.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                });
                al.setNegativeButton("取消", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        //do nothing
                    }
                });
                al.show();
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    datagramPacket = new DatagramPacket((COMMAND_LOGOUT + UserNickName).getBytes(), (COMMAND_LOGOUT + UserNickName).getBytes().length, InetAddress.getByName(IP), PORT_SER);
                    datagramSocket.send(datagramPacket);
                    MessageListener.interrupt();
                    datagramSocket.close();
                    ActivityManager activityMgr = (ActivityManager) MainActivity.this.getSystemService(Context.ACTIVITY_SERVICE);
                    if (activityMgr != null) {
                        activityMgr.killBackgroundProcesses(MainActivity.this.getPackageName());
                    }
                    System.exit(0);
                }catch (Exception e){
                    e.printStackTrace();
                }
            }
        }).start();
    }
}