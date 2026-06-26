import React from 'react';
import { motion } from 'motion/react';

interface WindowWrapperProps {
  id: string;
  title: string;
  isOpen: boolean;
  onClose: () => void;
  onMinimize: () => void;
  zIndex: number;
  onFocus: () => void;
  children: React.ReactNode;
  defaultX?: number;
  defaultY?: number;
  width?: string;
  height?: string;
  desktopRef: React.RefObject<HTMLDivElement | null>;
}

export default function WindowWrapper({
  id,
  title,
  isOpen,
  onClose,
  onMinimize,
  zIndex,
  onFocus,
  children,
  defaultX = 40,
  defaultY = 40,
  width = 'w-[620px]',
  height = 'h-auto',
  desktopRef
}: WindowWrapperProps) {
  if (!isOpen) return null;

  return (
    <motion.div
      drag
      dragMomentum={false}
      dragHandleClassName="window-titlebar"
      dragConstraints={desktopRef}
      initial={{ opacity: 0, scale: 0.97, x: defaultX, y: defaultY }}
      animate={{ opacity: 1, scale: 1 }}
      exit={{ opacity: 0, scale: 0.97 }}
      onPointerDown={onFocus}
      style={{ zIndex }}
      className={`absolute ${width} ${height} bg-[#FFFFFF] border border-[#E9E9E6] rounded-lg shadow-[0_12px_36px_rgba(0,0,0,0.06)] overflow-hidden flex flex-col`}
    >
      {/* Window Title Bar */}
      <div className="window-titlebar h-9 bg-[#F7F7F5] flex items-center px-4 border-b border-[#E9E9E6] select-none cursor-grab active:cursor-grabbing justify-between">
        <div className="flex items-center gap-2">
          {/* macOS Red / Yellow / Green window dots but minimalist */}
          <div className="flex gap-1.5 mr-2">
            <button
              onClick={(e) => {
                e.stopPropagation();
                onClose();
              }}
              className="w-3 h-3 rounded-full bg-[#EB5757]/15 hover:bg-[#EB5757] flex items-center justify-center text-[7px] text-[#EB5757] hover:text-white border border-[#EB5757]/35 cursor-pointer font-extrabold transition-all"
            >
              ×
            </button>
            <button
              onClick={(e) => {
                e.stopPropagation();
                onMinimize();
              }}
              className="w-3 h-3 rounded-full bg-[#F2C94C]/20 hover:bg-[#F2C94C] flex items-center justify-center text-[7px] text-[#D9730D] hover:text-white border border-[#F2C94C]/45 cursor-pointer font-extrabold transition-all"
            >
              -
            </button>
            <button
              onClick={(e) => {
                e.stopPropagation();
                onFocus(); // Maximizing is simplified as focusing
              }}
              className="w-3 h-3 rounded-full bg-[#27AE60]/15 hover:bg-[#27AE60] flex items-center justify-center text-[7px] text-[#0F7B5C] hover:text-white border border-[#27AE60]/35 cursor-pointer font-extrabold transition-all"
            >
              +
            </button>
          </div>
          <span className="text-[11px] text-[#37352F] font-bold font-sans">
            {title}
          </span>
        </div>
        <div className="text-[8px] text-[#9B9A97] uppercase tracking-wider font-mono">
          HYDROGEN LAYER
        </div>
      </div>

      {/* Window Content */}
      <div className="flex-1 overflow-auto max-h-[500px] text-[#37352F]">
        {children}
      </div>
    </motion.div>
  );
}
