# Application Development Folder

This folder contains the example codes (under the subfolders with prefix "hello\_\*") for the projects as well as the three main applications we developed for the project (under the subfoloders with prefix "il2212-\*"). 

Before your first usage please check and _understand_ the compilation/run scripts. For example, open the script in the `hello_world` project in a code editor (here `emacs`):

        cd path/to/il2212-lab/app/hello_world
        emacs run.sh &

**Attention:** the scripts are calling commands only recognized from within the **Nios II Shell**, so in order to use them you need to open a **Nios II Shell** terminal and type in the commands:

        cd path/to/il2206-lab/app/your-project
        bash run.sh

To call execute a script without specifying an interpreter, you need to change the status of the script file to be executable, and than you can invoke simply with `./run.sh`:

        chmod +x run.sh
        ./run.sh

