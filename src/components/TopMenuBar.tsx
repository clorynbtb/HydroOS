import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'motion/react';
import { Cpu, Layers, RefreshCw } from 'lucide-react';

interface TopMenuBarProps {
  onReboot: () => void;
  cpuUsage: number;
  ramUsage: string;
  onOpenWindow: (windowId: string) => void;
  onQuickAction: (action: string) => void;
}

/* Mac Classic dropdown menu style */
const macDropdownStyle: React.CSSProperties = {
  background: '#FFFFFF',
  border: '1px solid #5D4037',
  boxShadow: '2px 2px 0 #BCAAA4, 3px 3px 0 #D7CCC8',
  borderRadius: 0,
};

export default function TopMenuBar({ onReboot, cpuUsage, ramUsage, onOpenWindow, onQuickAction }: TopMenuBarProps) {
  const [activeMenu, setActiveMenu] = useState<string | null>(null);
  const [currentTime, setCurrentTime] = useState('');

  useEffect(() => {
    const updateTime = () => {
      const date = new Date();
      setCurrentTime(date.toLocaleString('en-US', {
        month: 'short',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: true,
      }));
    };
    updateTime();
    const interval = setInterval(updateTime, 1000);
    return () => clearInterval(interval);
  }, []);

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

  const menuItemClass = "w-full text-left px-3 py-1 text-[10.5px] font-sans cursor-pointer flex items-center gap-2 text-[#3E2723] hover:bg-[#3E2723] hover:text-white transition-colors";

  return (
    <header
      className="h-6 flex items-center px-2 justify-between text-[11px] select-none font-sans relative z-50"
      style={{
        /* Mac Classic menu bar: white with bottom border and subtle bevel */
        background: '#FFFFFF',
        borderBottom: '1px solid #5D4037',
        boxShadow: '0 1px 0 #EFEBE9',
      }}
    >
      <div className="flex items-center gap-0">

        {/* [H] Element Logo — Mac Classic Apple-logo equivalent */}
        <div className="relative mr-1">
          <button
            onClick={(e) => toggleMenu(e, 'hydrogen')}
            className="flex items-center justify-center cursor-pointer transition-colors"
            style={{
              width: 22,
              height: 18,
              background: activeMenu === 'hydrogen' ? '#3E2723' : '#EFEBE9',
              border: '1px solid #5D4037',
              boxShadow: activeMenu === 'hydrogen'
                ? 'inset 1px 1px 0 #5D4037, inset -1px -1px 0 #FFFFFF'
                : 'inset 1px 1px 0 #FFFFFF, inset -1px -1px 0 #8D6E63',
              color: activeMenu === 'hydrogen' ? '#FFFFFF' : '#3E2723',
            }}
          >
            {/* Stylized [H] block — atomic symbol for Hydrogen */}
            <div className="flex flex-col items-center justify-center" style={{ lineHeight: 1 }}>
              <span className="font-black font-mono text-[9px]" style={{ color: 'inherit', letterSpacing: '-0.5px' }}>H</span>
              <span className="font-bold font-mono" style={{ fontSize: 5, color: 'inherit', marginTop: -1 }}>1</span>
            </div>
          </button>

          <AnimatePresence>
            {activeMenu === 'hydrogen' && (
              <motion.div
                initial={{ opacity: 0, y: 3 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0, y: 3 }}
                transition={{ duration: 0.08 }}
                className="absolute left-0 mt-0.5 w-60 z-50"
                style={macDropdownStyle}
              >
                {/* About panel */}
                <div className="px-3 py-2 border-b border-[#D7CCC8] flex items-center gap-2.5">
                  {/* Atomic element box */}
                  <div
                    className="flex flex-col items-center justify-center"
                    style={{
                      width: 36, height: 36,
                      border: '1px solid #5D4037',
                      background: '#EFEBE9',
                      boxShadow: 'inset 1px 1px 0 #FFFFFF, inset -1px -1px 0 #8D6E63',
                    }}
                  >
                    <span className="font-mono font-black text-xs text-[#3E2723]">H</span>
                    <span className="font-mono text-[7px] text-[#8D6E63]">1</span>
                  </div>
                  <div>
                    <p className="font-black text-[#3E2723] text-[11px] font-sans">HydroOS v1.0.4</p>
                    <p className="text-[9px] text-[#8D6E63] font-mono">Hydrogen Core · Clang/LLD</p>
                  </div>
                </div>

                <div className="py-1 px-3 space-y-0.5">
                  {systemAboutInfo.map((info, idx) => (
                    <div key={idx} className="flex justify-between text-[9px] py-0.5 border-b border-[#F5F0EB] last:border-0">
                      <span className="text-[#8D6E63] font-sans">{info.label}</span>
                      <span className="text-[#3E2723] font-mono">{info.value}</span>
                    </div>
                  ))}
                </div>

                <div className="p-2 border-t border-[#D7CCC8]">
                  <button
                    onClick={() => onReboot()}
                    className="w-full py-1 text-[10px] font-bold font-sans flex items-center justify-center gap-1.5 cursor-pointer transition-colors"
                    style={{
                      background: '#EFEBE9',
                      border: '1px solid #5D4037',
                      color: '#3E2723',
                      boxShadow: 'inset 1px 1px 0 #FFFFFF, inset -1px -1px 0 #8D6E63',
                    }}
                  >
                    <RefreshCw className="w-3 h-3" /> Flash Reboot OS
                  </button>
                </div>
              </motion.div>
            )}
          </AnimatePresence>
        </div>

        {/* OS Name */}
        <span className="font-black text-[#3E2723] font-sans text-[11px] mr-2 ml-1">HydroOS</span>

        {/* Mac Classic Menu Items */}
        {['File', 'Edit', 'View', 'Docker', 'Window'].map((menu) => (
          <div key={menu} className="relative">
            <button
              onClick={(e) => toggleMenu(e, menu)}
              className="px-2 h-6 text-[11px] font-medium font-sans cursor-pointer flex items-center transition-colors"
              style={{
                background: activeMenu === menu ? '#3E2723' : 'transparent',
                color: activeMenu === menu ? '#FFFFFF' : '#3E2723',
              }}
            >
              {menu}
            </button>

            <AnimatePresence>
              {activeMenu === menu && (
                <motion.div
                  initial={{ opacity: 0, y: 3 }}
                  animate={{ opacity: 1, y: 0 }}
                  exit={{ opacity: 0, y: 3 }}
                  transition={{ duration: 0.08 }}
                  className="absolute left-0 mt-0.5 w-44 z-50 py-0.5"
                  style={macDropdownStyle}
                >
                  {menu === 'File' && (
                    <>
                      <button onClick={() => onOpenWindow('compiler')} className={menuItemClass}>
                        <Layers className="w-3 h-3" /> Browse Source Code
                      </button>
                      <div className="border-t border-[#D7CCC8] my-0.5" />
                      <button onClick={onReboot} className={menuItemClass}>
                        <RefreshCw className="w-3 h-3" /> System Restart
                      </button>
                    </>
                  )}
                  {menu === 'Edit' && (
                    <>
                      <button onClick={() => onOpenWindow('compiler')} className={menuItemClass}>Modify kernel.c</button>
                      <button onClick={() => onOpenWindow('compiler')} className={menuItemClass}>Modify oxygen.c</button>
                      <button onClick={() => onOpenWindow('compiler')} className={menuItemClass}>Modify db.c</button>
                    </>
                  )}
                  {menu === 'View' && (
                    <>
                      <button onClick={() => onOpenWindow('docker-manager')} className={menuItemClass}>Docker GUI Panel</button>
                      <button onClick={() => onOpenWindow('terminal')} className={menuItemClass}>TTY Terminal Console</button>
                    </>
                  )}
                  {menu === 'Docker' && (
                    <>
                      <button onClick={() => onQuickAction('start-all')} className={menuItemClass}>
                        ▶ Start All Containers
                      </button>
                      <button onClick={() => onQuickAction('stop-all')} className={menuItemClass}>
                        ⏸ Pause All Containers
                      </button>
                      <div className="border-t border-[#D7CCC8] my-0.5" />
                      <button onClick={() => onQuickAction('prune')} className={menuItemClass}>
                        ✕ Prune Stopped
                      </button>
                    </>
                  )}
                  {menu === 'Window' && (
                    <>
                      <button onClick={() => onOpenWindow('docker-manager')} className={menuItemClass}>Dubnium Manager v1.0.4</button>
                      <button onClick={() => onOpenWindow('terminal')} className={menuItemClass}>Terminal CLI</button>
                      <button onClick={() => onOpenWindow('compiler')} className={menuItemClass}>Clang Toolchain</button>
                    </>
                  )}
                </motion.div>
              )}
            </AnimatePresence>
          </div>
        ))}
      </div>

      {/* Right: Live diagnostics — Mac Classic system info style */}
      <div className="flex items-center gap-3 font-mono text-[#3E2723]">
        <span
          className="text-[9px] font-bold px-1.5 py-0.5 font-mono"
          style={{
            background: '#EFEBE9',
            border: '1px solid #D7CCC8',
            boxShadow: 'inset 1px 1px 0 #FFFFFF, inset -1px -1px 0 #8D6E63',
            color: '#3E2723',
          }}
        >
          ● LLD
        </span>
        <span className="flex items-center gap-1 text-[10px]">
          <Cpu className="w-3 h-3 text-[#8D6E63]" />
          <span className="font-bold text-[#3E2723]">{cpuUsage}%</span>
        </span>
        <span className="text-[10px] font-bold text-[#3E2723]">{ramUsage}</span>
        <span className="text-[9px] text-[#8D6E63]">{currentTime}</span>
      </div>
    </header>
  );
}
