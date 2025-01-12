import { Avatar, AvatarFallback, AvatarImage } from "@/components/ui/avatar";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu";
import { useAuth } from "@/hooks/useAuth";
import { User } from "lucide-react";
import React from "react";
import { Link } from "react-router";

const Navbar: React.FC = () => {
  const { logout } = useAuth();

  return (
    <nav className="bg-primary border-b  h-16">
      <div className="px-4 h-full mx-auto flex items-center justify-between">
        <div></div>
        <div className="flex items-center space-x-4">
          <DropdownMenu>
            <DropdownMenuTrigger className="flex items-center space-x-2 hover:bg-gray-100 rounded-full p-2">
              <div className="w-8 h-8 bg-blue-500 rounded-full flex items-center justify-center">
                <Avatar>
                  <AvatarImage
                    src="https://avatar.iran.liara.run/public"
                    alt="User avatar"
                  />
                  <AvatarFallback>USER</AvatarFallback>
                </Avatar>
              </div>
            </DropdownMenuTrigger>
            <DropdownMenuContent align="end">
              <DropdownMenuItem asChild>
                <Link to="/profile">Profile</Link>
              </DropdownMenuItem>
              <DropdownMenuItem className="text-red-600" onSelect={logout}>
                Logout
              </DropdownMenuItem>
            </DropdownMenuContent>
          </DropdownMenu>
        </div>
      </div>
    </nav>
  );
};

export default Navbar;
