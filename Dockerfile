FROM ubuntu:latest

# 安装必要的库和工具
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    git \
    libssl-dev \
    zlib1g-dev \
    openssh-server && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 设置SSH
RUN mkdir /var/run/sshd
RUN echo 'root:password' | chpasswd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH登录修复
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

# 导出环境变量
ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# 启动SSH
EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]


