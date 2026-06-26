export interface Container {
  id: string;
  image: string;
  status: 'running' | 'paused' | 'stopped';
  memory: string;
  cpu: number;
  port: string;
  created: string;
  appName: string;
  category: 'web' | 'database' | 'utility' | 'system';
}

export interface LogEntry {
  text: string;
  type: 'system' | 'info' | 'success' | 'warning' | 'error' | 'input';
  timestamp: string;
}

export interface CodeFile {
  name: string;
  path: string;
  language: string;
  content: string;
  description: string;
}

export type OSState = 'booting' | 'desktop' | 'rebooting';

export interface WindowState {
  id: string;
  title: string;
  isOpen: boolean;
  isMinimized: boolean;
  zIndex: number;
  x: number;
  y: number;
  width: number;
  height: number;
}
