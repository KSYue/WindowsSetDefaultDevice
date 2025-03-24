using System.Runtime.InteropServices;

namespace SetDefaultDevice
{
    internal class Program
    {

        public static bool SetDefaultDev(string deviceId, int? type = null)
        {
            if (string.IsNullOrEmpty(deviceId))
            {
                throw new ArgumentNullException("deviceId is empty or null");
            }
            IntPtr id = Marshal.StringToCoTaskMemUni(deviceId);
            if (id != IntPtr.Zero)
            {
                try
                {
                    bool isSuc = false;
                    if (type == null)
                    {
                        isSuc = SetDefaultDevice(id, 0) | SetDefaultDevice(id, 2);
                    }
                    else
                    {
                        isSuc = SetDefaultDevice(id, (int)type);
                    }
                    return isSuc;
                }
                catch (Exception e)
                {
                    throw e;
                }
                finally
                {
                    Marshal.FreeCoTaskMem(id);
                }
            }
            else
            {
                return false;
            }
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="type">0:Mic;1:Speaker</param>
        /// <returns></returns>
        public static DeviceData GetDefaultDev(int type)
        {
            var device = GetDefaultDevice((int)type);
            if (device.Id != IntPtr.Zero && device.Name != IntPtr.Zero)
            {
                try
                {
                    string? name = Marshal.PtrToStringUni(device.Name);
                    string? id = Marshal.PtrToStringUni(device.Id);
                    return new DeviceData { Name = name, Id = id };
                }
                finally
                {
                    Free(device.Name);
                    Free(device.Id);
                }
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="type">0:Mic;1:Speaker</param>
        /// <returns></returns>
        public static List<DeviceData> GetDevList(int type)
        {
            List<DeviceData> result = new List<DeviceData>();
            int deviceCount = 0;
            IntPtr devicesPtr = GetAudioDevices(ref deviceCount, type);
            if (deviceCount == 0 || devicesPtr == IntPtr.Zero)
                return result;
            int structSize = Marshal.SizeOf<DeviceInfo>();
            for (int i = 0; i < deviceCount; i++)
            {
                IntPtr current = IntPtr.Add(devicesPtr, i * structSize);
                DeviceInfo device = Marshal.PtrToStructure<DeviceInfo>(current);
                string? name = Marshal.PtrToStringUni(device.Name);
                string? id = Marshal.PtrToStringUni(device.Id);
                result.Add(new DeviceData { Name = name, Id = id });
            }
            FreeDevice(devicesPtr, deviceCount);
            return result;
        }

        static void Main(string[] args)
        {
            var defDevice = GetDefaultDev(0);
            Console.WriteLine("default device is :" + defDevice.Name + ",ID is :" + defDevice.Id);
            var list = GetDevList(0);
            int i = 0;
            foreach (var dev in list)
            {
                Console.WriteLine(i++ + ". device is :" + dev.Name + ",ID is :" + dev.Id);
            }
            Console.WriteLine("Please select the default device serial number to be set up.");
            var num =  Console.ReadLine();
            if(int.TryParse(num, out i))
            {
                if (SetDefaultDev(list[i].Id))
                {
                    Console.WriteLine("Success");
                }
                else
                {
                    Console.WriteLine("Fail");
                }
            }
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        private struct DeviceInfo
        {
            public IntPtr Name; // 对应 C++ 中的 wchar_t*
            public IntPtr Id;   // 对应 C++ 中的 wchar_t*
        }
        public class DeviceData
        {
            public string? Name { get; set; }
            public string? Id { get; set; }
        }

        [DllImport("DeviceManage.dll")]
        private extern static bool SetDefaultDevice(IntPtr id, int type);
        [DllImport("DeviceManage.dll")]
        private extern static DeviceInfo GetDefaultDevice(int type);
        [DllImport("DeviceManage.dll")]
        private extern static IntPtr GetAudioDevices(ref int deviceCount, int type);
        [DllImport("DeviceManage.dll")]
        private extern static void FreeDevice(IntPtr devices, int count);
        [DllImport("DeviceManage.dll")]
        private extern static void Free(IntPtr vchat);
    }
}
