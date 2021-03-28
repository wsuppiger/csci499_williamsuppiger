package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strings"
	"time"

	"github.com/golang/protobuf/proto"
	"github.com/golang/protobuf/ptypes"
	caw "github.com/williamsuppiger/csci499_williamsuppiger/src/go_caw/caw/protos"
	faz "github.com/williamsuppiger/csci499_williamsuppiger/src/go_caw/faz/protos"
	"golang.org/x/net/context"
	"google.golang.org/grpc"
)

// global flags
var (
	flag_hookall      = flag.Bool("hookall", false, "Hooks all Caw functions in the caw function class")
	flag_unhookall    = flag.Bool("unhookall", false, "Unhooks all Caw functions in the caw function class")
	flag_registeruser = flag.String("registeruser", "", "register user for a Caw account")
	flag_user         = flag.String("user", "", "Logs in as the given username")
	flag_caw          = flag.String("caw", "", "Creates a new caw with the given text")
	flag_reply        = flag.String("reply", "", "Indicates that the new caw is a reply to the given id")
	flag_follow       = flag.String("follow", "", "Starts following the given username")
	flag_read         = flag.String("read", "", "Reads the caw thread starting at the given id")
	flag_profile      = flag.Bool("profile", false, "Gets the user’s profile of following and followers")
)

// NOTE: these do not match the C++ Caw implementation order for enum in event_type.h
// and enum CommandInput in caw.cc, because this is a "mixed" enumeration so that
// there is no need for two separate enums in the same program.
type EventType int

const (
	kRegisteruser EventType = iota
	kCaw
	kFollow
	kRead
	kProfile
	kInvalidCommand
	kHookAll
	kUnhookAll
	kReply
)

// extracts and validates command provided by global flags
func getCommand() EventType {
	var hook, unhook, registeruser, user, caw, reply, follow, read, profile bool = *flag_hookall, *flag_unhookall, *flag_registeruser != "", *flag_user != "", *flag_caw != "",
		*flag_reply != "", *flag_follow != "", *flag_read != "", *flag_profile
	if !registeruser && !user && !caw && !reply && !follow && !read &&
		!profile {
		if hook && !unhook {
			return kHookAll // ./caw --hookall
		} else if !hook && unhook {
			return kUnhookAll // ./caw --unhookall
		}
	} else if !hook && !unhook {
		if registeruser && !user && !caw && !reply && !follow && !read &&
			!profile {
			return kRegisteruser // ./caw --registeruser <username>
		} else if !registeruser && user {
			if caw && !follow && !read && !profile {
				if caw && !reply {
					return kCaw // ./caw --user <username> --caw <caw text>
				} else if caw && reply {
					return kReply // ./caw --user <username> --caw <caw text>
					// --reply <reply caw id>
				}
			} else if !caw && follow && !read && !profile {
				return kFollow // ./caw --user <username> --follow <username>
			} else if !caw && !follow && read && !profile {
				return kRead // ./caw --user <username> --read <caw id>
			} else if !caw && !follow && !read && profile {
				return kProfile // ./caw --user <username> --profile
			}
		}
	}
	return kInvalidCommand
}

// gets event request through serializing and packing payload
func getEventRequest(request proto.Message, event EventType) *faz.EventRequest {
	serialized_payload, err := ptypes.MarshalAny(request)
	if err != nil {
		panic(err)
	}
	event_request := &faz.EventRequest{
		EventType: int32(event),
		Payload:   serialized_payload,
	}
	return event_request
}

func main() {
	flag.Parse()

	// GRPC
	var conn *grpc.ClientConn
	conn, err := grpc.Dial("0.0.0.0:50000", grpc.WithInsecure())
	if err != nil {
		fmt.Println("sorry, could not connect to Faz")
		log.Fatalf("did not connect: %s", err)
		conn.Close()
		os.Exit(1)
	}
	client := faz.NewFazServiceClient(conn)
	command := getCommand()
	event_map := map[EventType]string{
		kRegisteruser: "registeruser",
		kCaw:          "caw",
		kFollow:       "follow",
		kRead:         "read",
		kProfile:      "profile",
	}

	if command == kInvalidCommand {
		fmt.Println("Invalid command.  Use --help for info about the commands.")
		os.Exit(1)
	} else if command == kHookAll {
		for event_type, event_function := range event_map {
			_, err := client.Hook(context.Background(), &faz.HookRequest{
				EventType: int32(event_type), EventFunction: event_function,
			})
			if err != nil {
				fmt.Printf("There was an error \"%v\" hooking event %v\n", err, event_function)
				os.Exit(1)
			}
		}
		fmt.Println("All events successfully hooked.")
	} else if command == kUnhookAll {
		for event_type, event_function := range event_map {
			_, err := client.Unhook(context.Background(), &faz.UnhookRequest{
				EventType: int32(event_type),
			})
			if err != nil {
				fmt.Printf("There was an error \"%v\" unhooking event %v\n", err, event_function)
				os.Exit(1)
			}
		}
		fmt.Println("All events successfully unhooked.")
	} else if command == kRegisteruser {
		request := &caw.RegisteruserRequest{Username: *flag_registeruser}
		event_request := getEventRequest(request, kRegisteruser)
		_, err := client.Event(context.Background(), event_request)
		if err != nil {
			fmt.Println(grpc.ErrorDesc(err))
			os.Exit(1)
		} else {
			fmt.Println("registered user", *flag_registeruser)
		}
	} else if command == kCaw {
		request := &caw.CawRequest{Username: *flag_user, Text: *flag_caw}
		event_request := getEventRequest(request, kCaw)
		event_reply, err := client.Event(context.Background(), event_request)
		if err != nil {
			fmt.Println(grpc.ErrorDesc(err))
			os.Exit(1)
		} else {
			caw_reply := new(caw.CawReply)
			err := ptypes.UnmarshalAny(event_reply.GetPayload(), caw_reply)
			if err != nil {
				panic(err)
			}
			fmt.Println("created caw with ID:", string(caw_reply.GetCaw().Id))
		}
	} else if command == kReply {
		request := &caw.CawRequest{Username: *flag_user, Text: *flag_caw, ParentId: []byte(*flag_reply)}
		event_request := getEventRequest(request, kCaw)
		event_reply, err := client.Event(context.Background(), event_request)
		if err != nil {
			fmt.Println(grpc.ErrorDesc(err))
			os.Exit(1)
		} else {
			caw_reply := new(caw.CawReply)
			err := ptypes.UnmarshalAny(event_reply.GetPayload(), caw_reply)
			if err != nil {
				panic(err)
			}
			fmt.Println("created caw with ID:", string(caw_reply.GetCaw().Id), "in reply to", string(caw_reply.GetCaw().ParentId))
		}
	} else if command == kFollow {
		request := &caw.FollowRequest{Username: *flag_user, ToFollow: *flag_follow}
		event_request := getEventRequest(request, kFollow)
		_, err := client.Event(context.Background(), event_request)
		if err != nil {
			fmt.Println(grpc.ErrorDesc(err))
			os.Exit(1)
		} else {
			fmt.Println(*flag_user, "is now following", *flag_follow)
		}
	} else if command == kRead {
		request := &caw.ReadRequest{CawId: []byte(*flag_read)}
		event_request := getEventRequest(request, kRead)
		event_reply, err := client.Event(context.Background(), event_request)
		if err != nil {
			fmt.Println(grpc.ErrorDesc(err))
			os.Exit(1)
		} else {
			read_reply := new(caw.ReadReply)
			err := ptypes.UnmarshalAny(event_reply.GetPayload(), read_reply)
			if err != nil {
				panic(err)
			}
			caws := read_reply.Caws
			// Caws are returned in DFS pattern, so use stack-like structure to format parent indentation
			var parent_stack []caw.Caw
			for _, caw := range caws {
				for len(parent_stack) > 0 && string(caw.ParentId) != string(parent_stack[len(parent_stack)-1].Id) {
					parent_stack = parent_stack[:len(parent_stack)-1] // Pop
				}
				// print out caw in indented pretty format
				unix_time := time.Unix(caw.Timestamp.Seconds, 0)
				pretty_time := unix_time.Format(time.RFC3339)
				fmt.Println("[", pretty_time, "]", strings.Repeat("-", len(parent_stack)*4), "#"+string(caw.Id), caw.Username+":", caw.Text)
				parent_stack = append(parent_stack, *caw)
			}
		}
	} else if command == kProfile {
		request := &caw.ProfileRequest{Username: *flag_user}
		event_request := getEventRequest(request, kProfile)
		event_reply, err := client.Event(context.Background(), event_request)
		if err != nil {
			fmt.Println(grpc.ErrorDesc(err))
			os.Exit(1)
		} else {
			profile_reply := new(caw.ProfileReply)
			err := ptypes.UnmarshalAny(event_reply.GetPayload(), profile_reply)
			if err != nil {
				panic(err)
			}
			fmt.Println("followers: ", strings.Join(profile_reply.Followers, ", "))
			fmt.Println("following: ", strings.Join(profile_reply.Following, ", "))
		}
	}
}
