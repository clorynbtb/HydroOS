import React, { useState } from 'react';
import { CodeFile } from '../types';
import { FileCode, Play, AlertCircle, Save, Check } from 'lucide-react';

interface CodeEditorWindowProps {
  files: CodeFile[];
  onSaveFile: (fileName: string, content: string) => void;
  onCompile: (fileName: string) => void;
}

export default function CodeEditorWindow({ files, onSaveFile, onCompile }: CodeEditorWindowProps) {
  const [selectedFileIdx, setSelectedFileIdx] = useState(0);
  const [editorContent, setEditorContent] = useState(files[0].content);
  const [isSaved, setIsSaved] = useState(true);

  const activeFile = files[selectedFileIdx];

  const handleSelectFile = (idx: number) => {
    // Save current file if changed
    setSelectedFileIdx(idx);
    setEditorContent(files[idx].content);
    setIsSaved(true);
  };

  const handleTextChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    setEditorContent(e.target.value);
    setIsSaved(false);
  };

  const handleSave = () => {
    onSaveFile(activeFile.name, editorContent);
    setIsSaved(true);
  };

  const handleCompileTrigger = () => {
    // Ensure saved first
    if (!isSaved) {
      onSaveFile(activeFile.name, editorContent);
      setIsSaved(true);
    }
    onCompile(activeFile.name);
  };

  return (
    <div className="flex h-[360px] bg-[#FFFFFF] text-[#37352F] font-sans text-xs select-text">
      {/* File Sidebar */}
      <div className="w-48 border-r border-[#E9E9E6] bg-[#F7F7F5] flex flex-col justify-between select-none">
        <div className="p-3 space-y-2">
          <div className="text-[10px] text-[#9B9A97] uppercase tracking-wider font-bold border-b border-[#E9E9E6] pb-2 mb-2 font-sans">
            Workspace (Clang)
          </div>
          {files.map((file, idx) => (
            <button
              key={file.name}
              onClick={() => handleSelectFile(idx)}
              className={`w-full flex items-center gap-2 px-2.5 py-1.5 rounded-md text-left transition-all cursor-pointer ${
                selectedFileIdx === idx
                  ? 'bg-[#EDF6F3] text-[#0F7B5C] border border-[#D0E7E1] font-semibold'
                  : 'hover:bg-[#EFEFED] text-[#37352F] border border-transparent'
              }`}
            >
              <FileCode className="w-3.5 h-3.5" />
              <div className="overflow-hidden text-ellipsis whitespace-nowrap">
                <span className="font-mono text-[11px]">{file.name}</span>
                <p className="text-[8px] text-[#9B9A97] font-normal font-sans">{file.name === 'kernel.c' ? 'Hydrogen' : file.name === 'oxygen.c' ? 'Oxygen' : 'Dubnium'}</p>
              </div>
            </button>
          ))}
        </div>

        <div className="p-3 border-t border-[#E9E9E6] text-[9.5px] text-[#9B9A97] space-y-1 bg-[#F7F7F5] font-mono">
          <p className="font-bold text-[#37352F] uppercase font-sans">Clang Target Specs</p>
          <p>ARCH: x86_64-none-elf</p>
          <p>LINK: lld -m elf_x86_64</p>
          <p>FLAGS: -ffreestanding -O2</p>
        </div>
      </div>

      {/* Editor Main Canvas */}
      <div className="flex-1 flex flex-col justify-between bg-[#FFFFFF]">
        {/* Editor Ribbon Bar */}
        <div className="h-9 border-b border-[#E9E9E6] bg-[#F7F7F5] px-4 flex items-center justify-between select-none">
          <div className="flex items-center gap-2">
            <span className="text-[#37352F] font-bold font-mono">{activeFile.path}</span>
            {!isSaved && (
              <span className="w-1.5 h-1.5 rounded-full bg-[#D9730D] animate-pulse" title="Unsaved alterations" />
            )}
          </div>

          <div className="flex items-center gap-2">
            {/* Save Button */}
            <button
              onClick={handleSave}
              disabled={isSaved}
              className={`px-2.5 py-1 rounded border flex items-center gap-1.5 text-[10px] cursor-pointer transition-all ${
                isSaved
                  ? 'bg-[#F7F7F5] border-[#E9E9E6] text-[#9B9A97] cursor-not-allowed font-medium'
                  : 'bg-[#FFFFFF] border-[#E9E9E6] text-[#37352F] hover:bg-[#F7F7F5]'
              }`}
            >
              {isSaved ? <Check className="w-3 h-3 text-[#0F7B5C]" /> : <Save className="w-3 h-3 text-[#37352F]" />}
              {isSaved ? 'Saved' : 'Save'}
            </button>

            {/* Compile & Reboot Button */}
            <button
              onClick={handleCompileTrigger}
              className="bg-[#0F7B5C] hover:bg-[#0C6249] border border-[#0F7B5C] text-white font-bold px-3 py-1 rounded flex items-center gap-1.5 text-[10px] cursor-pointer transition-all shadow-sm"
            >
              <Play className="w-3 h-3 fill-white text-white" />
              Compile & Boot
            </button>
          </div>
        </div>

        {/* Text Area Content */}
        <div className="flex-grow relative flex">
          {/* Mock Line Numbers */}
          <div className="w-8 select-none text-right pr-2.5 py-3 text-[10px] text-[#9B9A97] border-r border-[#E9E9E6] bg-[#F7F7F5] leading-relaxed font-mono">
            {Array.from({ length: editorContent.split('\n').length || 1 }).map((_, i) => (
              <div key={i}>{i + 1}</div>
            ))}
          </div>

          {/* Code Text Editor */}
          <textarea
            value={editorContent}
            onChange={handleTextChange}
            spellCheck="false"
            className="flex-1 bg-transparent text-[#37352F] font-mono text-[11px] p-3 leading-relaxed outline-none resize-none border-none caret-[#37352F]"
          />
        </div>

        {/* Info ribbon */}
        <div className="h-6 bg-[#F7F7F5] border-t border-[#E9E9E6] px-3 flex items-center justify-between text-[9px] text-[#9B9A97] select-none font-sans">
          <span className="flex items-center gap-1">
            <AlertCircle className="w-3.5 h-3.5 text-[#0F7B5C]" />
            <span className="font-medium">{activeFile.description}</span>
          </span>
          <span className="font-mono">UTF-8 • LF • C (CLANG)</span>
        </div>
      </div>
    </div>
  );
}
