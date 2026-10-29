struct Counter_struct{
int n;
};
struct Holder_struct{
Counter* inner;       // a pointer member
Counter own;          // a value member

};
struct Seq_struct{
int cur;
};
struct Puller_struct{
};
