
#include <iostream> //io streams for methods
#include <fstream>
using namespace std ;

typedef int int32; // 4-byte int
typedef char byte; // alias char as byte

class MyFileSystem

{
public:
   void LoadDisk();
   void DumpData();
   bool Create(const char *name, int32 size);
   bool Delete(const char *name);
   void ReadBlock(int32 blockPtr, char *buf);
   void WriteBlock(int32 blockPtr, const char *buf);
   bool Read(const char *name, int32 blockNum, char *buf);
   bool Write(const char *name, int32 blockNum, const char *buf);
   void ls();

   string disk0; //disk0

private:
   byte free_block[128]; //if the ith byte is 0 then the block is free, else it is in use

   /* Inode block : total size = 48 bytes*/
   struct Inode
   {
       string name;            // file name
       int32 size;               // file size
       int32 blockPointers[8]; // direct block pointers
       int32 used;               // 0 = inode is free; 1 = in use 
   };

   Inode inodes[16];
   void Commit(int inode);
   friend ifstream& operator>>(std::ifstream&, MyFileSystem::Inode&);
   friend ofstream& operator<<(std::ofstream&, MyFileSystem::Inode&);
};

ifstream& operator >> (ifstream& inn, MyFileSystem::Inode& dest)
{
   char name[9];
   inn.read(name, 8);
   name[8] = 0;
   dest.name = string(name);
   inn >> dest.size;
   for(int i=0;i<8;i++)
       inn >> dest.blockPointers[i];
   inn >> dest.used;
}

ofstream& operator << (ofstream& onn, MyFileSystem::Inode& src)
{
   onn.write(src.name.c_str(), 8);
   onn << src.size;
   for(int i=0;i<8;i++)
       onn << src.blockPointers[i];
   onn << src.used;
}

void MyFileSystem::LoadDisk()
{
   ifstream diskReader(disk0);
   // read free-block list
   diskReader.read(free_block, 128);  
   // read inodes
   for(int i = 0; i < 16; i++)
       diskReader >> inodes[i];
  
   diskReader.close();
}

void MyFileSystem::DumpData()
{
   ofstream diskWriter(disk0);
  
   // write free-block list
   diskWriter.write(free_block, 128);
   // write inodes
   for(int i = 0; i < 16; i++)
       diskWriter << inodes[i];
  
   diskWriter.close();
}

bool MyFileSystem::Create(const char *name, int32 size)
/*create a new file with this name and with these many blocks. (We shall assume that the file size is specified at file creation time and the file does not grow or shrink from this point on) */
{
   if(size < 1 || size > 8)
   {
       cout << "Invalid block size specified!\n";
       return false;
   }
  
   string filename(name);
   int chosenInode = 16;
   for(int i=0;i<16; i++)
       if(inodes[i].used == 1 && filename == inodes[i].name)
       {
           cout << "A file by name " << filename << " already exists!\n";
           return false;
       }
       else if (inodes[i].used == 0)
       {
           chosenInode = i;
           break;
       }
      
   if(chosenInode == 16)
   {
       cout << "No space left on disk!\n";
       return false;
   }
   inodes[chosenInode].name = filename;
   inodes[chosenInode].size = size;
   inodes[chosenInode].used = 1;
   for(int i = 0; i < size; i++)
       inodes[chosenInode].blockPointers[i] = (chosenInode+1)*1024 + i*127;
   Commit(chosenInode);
   return true;
}

bool MyFileSystem::Delete(const char *name)
/* find the file with matching name then deletes it */
{
   string filename(name);
   for(int i=0;i<16; i++)
       if(filename == inodes[i].name)
       {
           cout << "Deleting file : " << filename << endl;
           inodes[i].used = 0;
           Commit(i);
           return true;
       }
      
   cout << filename << " : File not found\n";
   return false;
}

void MyFileSystem::Commit(int inode)
{
ofstream diskWriter (disk0 , ofstream::binary | ofstream::app);
   diskWriter.seekp(128 + 48*(inode));
   diskWriter << inodes[inode];
   diskWriter.close();
}

void MyFileSystem::ReadBlock(int32 blockPtr, char *buf)
{
   ifstream diskReader(disk0, ifstream::binary);
   diskReader.seekg(blockPtr, diskReader.beg);
   diskReader.read(buf, 1024);
   diskReader.close();          
}

void MyFileSystem::WriteBlock(int32 blockPtr, const char *buf)
{
   ofstream diskWriter(disk0, ofstream::binary | ofstream::app);
   diskWriter.seekp(blockPtr, diskWriter.beg);
   diskWriter.write(buf, 1024);
   diskWriter.close();          
}

bool MyFileSystem::Read(const char* name, int32 blockNum, char *buf)
/* read the specified block from this file into the specified buffer; blockNum can range from 0 to 7. */
{
   if(blockNum < 0 || blockNum > 7)
   {
       cout << "Invalid block number specified!\n";
       return false;
   }
   string filename(name);
   for(int i=0;i<16; i++)
       if(filename == inodes[i].name)
       {
           if(inodes[i].size < blockNum + 1)
           {
               cout << "Invalid block number specified!\n";
               return false;
           }
           cout << "Copying block# " << blockNum << " from " << filename << " from disk " << endl;          
           ReadBlock(inodes[i].blockPointers[blockNum], buf);          
           return true;
       }
      
   cout << filename << " : File not found\n";
   return false;
}

bool MyFileSystem::Write(const char *name, int32 blockNum, const char *buf)
/* write the data in the buffer to the specified block in this file. */
{
   if(blockNum < 0 || blockNum > 7)
   {
       cout << "Invalid block number specified!\n";
       return false;
   }
   string filename(name);
   for(int i=0; i<16; i++)
       if(filename == inodes[i].name)
       {
           if(inodes[i].size < blockNum + 1)
           {
               cout << "Invalid block number specified!\n";
               return false;
           }
           cout << "Writing block# " << blockNum << " from " << filename << " to disk " << endl;          
           WriteBlock(inodes[i].blockPointers[blockNum], buf);          
           return true;
       }
      
   cout << filename << " : File not found\n";
   return false;  
}


void MyFileSystem::ls()
/* list all file names and */
{
   cout << "Listing all files in the file system!" << endl;
   for (int i = 0; i < 16; i++)
   {
       if (inodes[i].used == 1)
           cout << inodes[i].name << endl;
   }
}


int main(int argc, char* argv[])
{
   if (argc < 2)
   {
       cout << "Syntax : " << argv[0] <<" <input file>" << endl;
       return 1;
   }
  
   ifstream input(argv[1]);
   if (!input.is_open())
   {
       cout << argv[1] << " : file not found " << endl;
       return 1;
   }
  
   MyFileSystem unix;
   input >> unix.disk0;
  
   unix.LoadDisk();
  
   string cmd;
   string filename;
   int blockNum;
   char buf[1024];
  
   while(! input.eof())
   {
       input >> cmd;
      
       if(cmd == "L")
       {
           unix.ls();
           continue;
       }
      
       input >> filename ;
      
       if(cmd == "D")
       {
           unix.Delete(filename.c_str());
           continue;
       }
      
       input >> blockNum;
      
       if(cmd == "C")
       {
           unix.Create(filename.c_str(), blockNum);
       }
       else if(cmd == "W")
       {
           unix.Write(filename.c_str(), blockNum, buf);
       }
       else if(cmd == "R")
       {
           unix.Read(filename.c_str(), blockNum, buf);
       }
   }
   input.close();
   unix.DumpData();
   return 0;
}

