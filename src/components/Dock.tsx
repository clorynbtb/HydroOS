import React from 'react';
import { motion } from 'motion/react';
import { Terminal, Code, Trash2 } from 'lucide-react';

interface DockProps {
  onOpenWindow: (windowId: string) => void;
  activeWindows: { [key: string]: boolean };
  onClearContainers: () => void;
}

export default function Dock({ onOpenWindow, activeWindows, onClearContainers }: DockProps) {
  const dockItems = [
    {
      id: 'docker-manager',
      label: 'Dubnium Docker Manager',
      symbol: 'Db',
      isCustom: true,
      textColor: 'text-[#0F7B5C]', // Emerald Accent
      activeDotColor: 'bg-[#0F7B5C]',
    },
    {
      id: 'compiler',
      label: 'CLANG x86_64 Compiler',
      symbol: <Code className="w-5 h-5 text-[#D9730D]" />, // Amber Accent
      isCustom: false,
      textColor: 'text-[#D9730D]',
      activeDotColor: 'bg-[#D9730D]',
    },
    {
      id: 'terminal',
      label: 'TTY Terminal Console',
      symbol: <Terminal className="w-5 h-5 text-[#37352F]" />, // Charcoal
      isCustom: false,
      textColor: 'text-[#37352F]',
      activeDotColor: 'bg-[#37352F]',
    },
  ];

  return (
    <footer className="h-20 flex items-center justify-center mb-4 select-none font-sans relative z-40">
      <div className="bg-[#FFFFFF] border border-[#E9E9E6] rounded-2xl p-2.5 flex gap-3 shadow-[0_8px_32px_rgba(0,0,0,0.06)] items-end relative">
        {dockItems.map((item) => {
          const isOpen = activeWindows[item.id];
          return (
            <div key={item.id} className="relative group flex flex-col items-center">
              {/* Floating Tooltip */}
              <div className="absolute -top-12 bg-[#FFFFFF] border border-[#E9E9E6] text-[10px] text-[#37352F] font-medium px-2.5 py-1 rounded-md opacity-0 group-hover:opacity-100 transition-opacity duration-200 pointer-events-none whitespace-nowrap shadow-[0_4px_12px_rgba(0,0,0,0.06)] z-50">
                {item.label}
              </div>

              {/* Icon Container */}
              <motion.button
                onClick={() => onOpenWindow(item.id)}
                whileHover={{ y: -8, scale: 1.05 }}
                whileTap={{ scale: 0.95 }}
                className="w-12 h-12 rounded-xl flex items-center justify-center border border-[#E9E9E6] bg-[#F7F7F5] relative cursor-pointer shadow-sm hover:bg-[#EFEFED] transition-all"
              >
                {item.isCustom ? (
                  <span className="font-mono font-bold text-base tracking-tighter text-[#0F7B5C]">
                    {item.symbol}
                  </span>
                ) : (
                  item.symbol
                )}
              </motion.button>

              {/* Active Application Status Indicator Dot */}
              <div className="h-1.5 mt-1 flex items-center justify-center">
                {isOpen && (
                  <motion.div
                    layoutId={`dock-dot-${item.id}`}
                    className={`w-1.5 h-1.5 rounded-full ${item.activeDotColor}`}
                  />
                )}
              </div>
            </div>
          );
        })}

        {/* Separator */}
        <div className="w-[1px] h-8 bg-[#E9E9E6] self-center mx-1 mb-2.5"></div>

        {/* Trash Can / Purge Docker Container */}
        <div className="relative group flex flex-col items-center">
          <div className="absolute -top-12 bg-[#FFFFFF] border border-[#E9E9E6] text-[10px] text-[#37352F] font-medium px-2.5 py-1 rounded-md opacity-0 group-hover:opacity-100 transition-opacity duration-200 pointer-events-none whitespace-nowrap shadow-[0_4px_12px_rgba(0,0,0,0.06)] z-50">
            Prune Dormant Containers
          </div>
          <motion.button
            onClick={() => onClearContainers()}
            whileHover={{ y: -8, scale: 1.05, rotate: [0, -5, 5, -5, 0] }}
            whileTap={{ scale: 0.95 }}
            className="w-12 h-12 bg-[#F7F7F5] rounded-xl flex items-center justify-center border border-[#E9E9E6] text-[#9B9A97] hover:text-[#EB5757] hover:bg-[#FFF0F0] hover:border-[#FADCDD] cursor-pointer shadow-sm mb-2.5 transition-all"
          >
            <Trash2 className="w-5 h-5" />
          </motion.button>
        </div>
      </div>
    </footer>
  );
}
