@0x88ad9f8d69e21c42;

struct AsyncRequest {
    replyId @0 : Text;
    union {
        getServiceAsync :group{
            serviceName @1 : Text;
        }
        registerServiceAsync :group{
            useLater @2 : Text;
        }
    }
}