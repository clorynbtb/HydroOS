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
      style={{
        zIndex,
        /* Mac Classic outer frame: dark brown border + white highlight bevel */
        boxShadow: `
          inset 1px 1px 0 #FFFFFF,
          inset -1px -1px 0 #5D4037,
          2px 2px 0 #D7CCC8,
          3px 3px 0 #BCAAA4
        `,
      }}
      className={`absolute ${width} ${height} overflow-hidden flex flex-col border border-[#5D4037]`}
      // Sharp square corners — Mac Classic has NO rounded corners
    >
      {/* Mac Classic Title Bar — striped background with centered title */}
      <div
        className="window-titlebar h-6 flex items-center px-2 select-none cursor-grab active:cursor-grabbing justify-between relative"
        style={{
          /* Mac System 7 horizontal stripe pattern */
          background: `repeating-linear-gradient(
            to bottom,
            #EFEBE9 0px,
            #EFEBE9 2px,
            #D7CCC8 2px,
            #D7CCC8 3px
          )`,
          borderBottom: '1px solid #5D4037',
        }}
      >
        {/* Mac Classic Close Box — square, not circle */}
        <div className="flex items-center gap-1.5 shrink-0 z-10">
          <button
            onClick={(e) => { e.stopPropagation(); onClose(); }}
            title="Close"
            style={{
              width: 12,
              height: 12,
              background: '#EFEBE9',
              border: '1px solid #5D4037',
              boxShadow: 'inset 1px 1px 0 #FFFFFF, inset -1px -1px 0 #8D6E63',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              cursor: 'pointer',
              flexShrink: 0,
            }}
            className="text-[7px] font-black text-[#5D4037] hover:bg-[#D7CCC8] transition-colors"
          >
            ×
          </button>
          <button
            onClick={(e) => { e.stopPropagation(); onMinimize(); }}
            title="Minimize"
            style={{
              width: 12,
              height: 12,
              background: '#EFEBE9',
              border: '1px solid #5D4037',
              boxShadow: 'inset 1px 1px 0 #FFFFFF, inset -1px -1px 0 #8D6E63',
              display: 'flex',
              alignItems: 'center',
              justifyContent: 'center',
              cursor: 'pointer',
              flexShrink: 0,
            }}
            className="text-[7px] font-black text-[#5D4037] hover:bg-[#D7CCC8] transition-colors"
          >
            –
          </button>
        </div>

        {/* Title centered over stripes */}
        <span
          className="absolute inset-0 flex items-center justify-center text-[10px] font-bold font-sans pointer-events-none px-16"
          style={{ color: '#3E2723', letterSpacing: '0.02em' }}
        >
          {title}
        </span>

        <div className="text-[8px] font-mono shrink-0 z-10" style={{ color: '#8D6E63' }}>
          [H]
        </div>
      </div>

      {/* Window Content — 1px inset border on all sides for depth */}
      <div
        className="flex-1 overflow-auto max-h-[500px] text-[#37352F]"
        style={{
          background: '#FFFFFF',
          borderTop: '1px solid #FFFFFF',
        }}
      >
        {children}
      </div>
    </motion.div>
  );
}
