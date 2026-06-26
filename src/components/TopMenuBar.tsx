import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { Terminal, Shield, Cpu, Layers, RefreshCw } from 'lucide-react';

interface TopMenuBarProps {
  onReboot: () => void;
  cpuUsage: number;
  ramUsage: string;
  onOpenWindow: (windowId: string) => void;
  onQuickAction: (action: string) => void;
}

export default function TopMenuBar({ onReboot, cpuUsage, ramUsage, onOpenWindow, onQuickAction }: TopMenuBarProps) {
  const [activeMenu, setActiveMenu] = useState<string | null>(null);
  const [currentTime, setCurrentTime] = useState('');

  // Clock Update
  useEffect(() => {
    const updateTime = () => {
      const date = new Date();
      const options: Intl.DateTimeFormatOptions = {
        month: 'short',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: true,
      };
      setCurrentTime(date.toLocaleString('en-US', options));
    };
    updateTime();
    const interval = setInterval(updateTime, 1000);
    return () => clearInterval(interval);
  }, []);

  // Handle clicking outside to close menus
  useEffect(() => {
    if (!activeMenu) return;
    const handleOutsideClick = () => setActiveMenu(null);
    window.addEventListener('click', handleOutsideClick);
    return () => window.removeEventListener('click', handleOutsideClick);
  }, [activeMenu]);

  const toggleMenu = (e: React.MouseEvent, menuName: string) => {
    e.stopPropagation();
    setActiveMenu(prev => (prev === menuName ? null : menuName));
  };

  const systemAboutInfo = [
    { label: 'OS Identifier', value: 'HydroOS x86_64' },
    { label: 'Kernel Engine', value: 'Hydrogen Core (kernel.c)' },
    { label: 'Bootloader', value: 'Limine Protocol v5.x' },
    { label: 'UI Subsystem', value: 'Oxygen Graphics (oxygen.c)' },
    { label: 'Virtualization', value: 'Dubnium Docker Engine (db.c)' },
    { label: 'Compiler Target', value: 'clang -target x86_64-none-elf' },
    { label: 'Linker Mode', value: 'lld -m elf_x86_64' },
  ];

  return (
    <header className="h-7 bg-[#FFFFFF] border-b border-[#E9E9E6] flex items-center px-4 justify-between text-[11px] select-none font-sans text-[#37352F] relative z-50">
      <div className="flex items-center gap-3">
        {/* Minimalist Hydrogen Logo */}
        <div className="relative">
          <button
            onClick={(e) => toggleMenu(e, 'hydrogen')}
            className={`font-mono font-extrabold cursor-pointer px-2 py-0.5 rounded transition-all ${activeMenu === 'hydrogen' ? 'bg-[#EFEFED] text-[#37352F]' : 'hover:bg-[#F7F7F5] text-[#37352F]'}`}
          >
            H
          </button>
          
          <AnimatePresence>
            {activeMenu === 'hydrogen' && (
              <motion.div
                initial={{ opacity: 0, y: 5 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0, y: 5 }}
                className="absolute left-0 mt-1.5 w-64 bg-[#FFFFFF] border border-[#E9E9E6] rounded-lg shadow-[0_8px_24px_rgba(0,0,0,0.08)] p-3.5 z-50 text-[#37352F] space-y-3"
              >
                <div className="flex items-center gap-3 border-b border-[#E9E9E6] pb-2.5">
                  <div className="w-9 h-9 border border-[#37352F] bg-[#F7F7F5] rounded flex items-center justify-center font-bold text-[#37352F] text-lg">
                    H
                  </div>
                  <div>
                    <h4 className="font-bold text-[#37352F] text-xs">HydroOS v1.0.4</h4>
                    <p className="text-[10px] text-[#9B9A97]">Hydrogen Kernel & Docker</p>
                  </div>
                </div>

                <div className="space-y-1.5">
                  {systemAboutInfo.map((info, idx) => (
                    <div key={idx} className="flex justify-between text-[9.5px]">
                      <span className="text-[#9B9A97]">{info.label}:</span>
                      <span className="text-[#37352F] font-mono text-right">{info.value}</span>
                    </div>
                  ))}
                </div>

                <button
                  onClick={() => onReboot()}
                  className="w-full py-1.5 bg-[#FFF0F0] border border-[#FADCDD] hover:bg-[#FCE8E8] text-[#EB5757] rounded-md text-[10px] flex items-center justify-center gap-1.5 cursor-pointer transition-all font-semibold uppercase"
                >
                  <RefreshCw className="w-3 h-3 animate-spin-slow" />
                  Flash Reboot OS
                </button>
              </motion.div>
            )}
          </AnimatePresence>
        </div>

        {/* OS Title */}
        <span className="font-bold text-[#37352F] mr-3 font-sans">HydroOS</span>

        {/* Menu Options */}
        {['File', 'Edit', 'View', 'Docker', 'Window'].map((menu) => (
          <div key={menu} className="relative">
            <button
              onClick={(e) => toggleMenu(e, menu)}
              className={`cursor-pointer px-2.5 py-0.5 rounded font-medium transition-all ${
                activeMenu === menu ? 'bg-[#EFEFED] text-[#37352F]' : 'text-[#37352F] hover:bg-[#F7F7F5]'
              }`}
            >
              {menu}
            </button>

            <AnimatePresence>
              {activeMenu === menu && (
                <motion.div
                  initial={{ opacity: 0, y: 5 }}
                  animate={{ opacity: 1, y: 0 }}
                  exit={{ opacity: 0, y: 5 }}
                  className="absolute left-0 mt-1.5 w-44 bg-[#FFFFFF] border border-[#E9E9E6] rounded-lg shadow-[0_8px_24px_rgba(0,0,0,0.08)] py-1 z-50 text-[#37352F]"
                >
                  {menu === 'File' && (
                    <>
                      <button onClick={() => onOpenWindow('compiler')} className="w-full text-left px-3 py-1.5 hover:bg-[#EDF6F3] hover:text-[#0F7B5C] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        <Layers className="w-3 h-3 text-[#9B9A97]" /> Browse Source Code
                      </button>
                      <hr className="border-[#E9E9E6] my-1" />
                      <button onClick={onReboot} className="w-full text-left px-3 py-1.5 hover:bg-[#FFF0F0] hover:text-[#EB5757] flex items-center gap-2 text-[#EB5757] cursor-pointer text-[10.5px]">
                        <RefreshCw className="w-3 h-3" /> System Restart
                      </button>
                    </>
                  )}
                  {menu === 'Edit' && (
                    <>
                      <button onClick={() => onOpenWindow('compiler')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        Modify kernel.c
                      </button>
                      <button onClick={() => onOpenWindow('compiler')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        Modify oxygen.c
                      </button>
                      <button onClick={() => onOpenWindow('compiler')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        Modify db.c
                      </button>
                    </>
                  )}
                  {menu === 'View' && (
                    <>
                      <button onClick={() => onOpenWindow('docker-manager')} className="w-full text-left px-3 py-1.5 hover:bg-[#EDF6F3] hover:text-[#0F7B5C] cursor-pointer text-[10.5px]">
                        Docker GUI Panel
                      </button>
                      <button onClick={() => onOpenWindow('terminal')} className="w-full text-left px-3 py-1.5 hover:bg-[#EDF6F3] hover:text-[#0F7B5C] cursor-pointer text-[10.5px]">
                        TTY Terminal Console
                      </button>
                    </>
                  )}
                  {menu === 'Docker' && (
                    <>
                      <button onClick={() => onQuickAction('start-all')} className="w-full text-left px-3 py-1.5 hover:bg-[#EDF6F3] hover:text-[#0F7B5C] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        <span>▶</span> Start All Containers
                      </button>
                      <button onClick={() => onQuickAction('stop-all')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] text-[#9B9A97] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        <span>⏸</span> Pause All Containers
                      </button>
                      <button onClick={() => onQuickAction('prune')} className="w-full text-left px-3 py-1.5 hover:bg-[#FFF0F0] hover:text-[#EB5757] text-[#EB5757] flex items-center gap-2 cursor-pointer text-[10.5px]">
                        <span>🗑</span> Prune Stopped
                      </button>
                    </>
                  )}
                  {menu === 'Window' && (
                    <>
                      <button onClick={() => onOpenWindow('docker-manager')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] cursor-pointer text-[10.5px]">
                        Dubnium Manager v1.0.4
                      </button>
                      <button onClick={() => onOpenWindow('terminal')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] cursor-pointer text-[10.5px]">
                        Terminal CLI
                      </button>
                      <button onClick={() => onOpenWindow('compiler')} className="w-full text-left px-3 py-1.5 hover:bg-[#F7F7F5] cursor-pointer text-[10.5px]">
                        Clang Toolchain Compiler
                      </button>
                    </>
                  )}
                </motion.div>
              )}
            </AnimatePresence>
          </div>
        ))}
      </div>

      {/* Right side live diagnostics */}
      <div className="flex items-center gap-4 text-[#9B9A97] font-mono">
        <span className="text-[#0F7B5C] font-semibold hidden sm:inline-block">● [LLD/CLANG]</span>
        <span className="flex items-center gap-1.5">
          <Cpu className="w-3 h-3 text-[#9B9A97]" />
          <span className="text-[#37352F] font-bold">{cpuUsage}%</span> CPU
        </span>
        <span className="text-[#37352F] font-medium">{ramUsage} RAM</span>
        <span className="text-[#9B9A97] text-[10px]">{currentTime}</span>
      </div>
    </header>
  );
}
