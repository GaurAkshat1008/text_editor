import React from "react";
import { useLocation, Link } from "react-router-dom";
import { cn } from "@/lib/utils";
import {
  FileText,
  FolderOpen,
  Settings,
  UserCircle,
  Search,
  PlusCircle,
} from "lucide-react";

const navigation = [
  { name: "My Documents", href: "/documents", icon: FileText },
  { name: "New Document", href: "/documents/new", icon: PlusCircle },
  { name: "Shared with Me", href: "/shared", icon: FolderOpen },
];

const Sidebar: React.FC = () => {
  const location = useLocation();

  return (
    <aside className="bg-white w-64 h-full border-r flex flex-col">
      <nav className="flex-1 p-4 space-y-1">
        {navigation.map((item) => {
          const Icon = item.icon;
          const isActive = location.pathname === item.href;

          return (
            <Link
              key={item.name}
              to={item.href}
              className={cn(
                "flex items-center space-x-3 px-3 py-2 rounded-lg transition-colors",
                isActive
                  ? "bg-primary/10 text-primary"
                  : "text-gray-700 hover:bg-gray-100"
              )}
            >
              <Icon className="h-5 w-5" />
              <span>{item.name}</span>
            </Link>
          );
        })}
      </nav>

      {/* Bottom section */}
      <div className="p-4 border-t">
        <Link
          to="/settings"
          className="flex items-center space-x-3 px-3 py-2 text-gray-700 hover:bg-gray-100 rounded-lg"
        >
          <Settings className="h-5 w-5" />
          <span>Settings</span>
        </Link>
      </div>
    </aside>
  );
};

export default Sidebar;
